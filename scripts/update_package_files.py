#!/usr/bin/env python3

import argparse
import hashlib
import json
import pathlib
import re
import sys

parser = argparse.ArgumentParser()

parser.add_argument("--package", required=True)
parser.add_argument("--repo", required=True)
parser.add_argument("--version", required=True)
parser.add_argument("--commit", required=True)
parser.add_argument("--archive", required=True)
parser.add_argument(
    "--submodules-manifest",
    required=False,
    default=None,
    help="Path to a JSON file: a list of {name, commit, archive} objects "
    "describing vendored submodules to update in portfile.cmake's "
    "SUBMODULE_SPECS. Optional -- omit (or point at a file with an empty "
    "list) for projects with no SUBMODULE_SPECS entries to update. Each "
    "name must already have an entry in portfile.cmake's SUBMODULE_SPECS.",
)
parser.add_argument("--root-dir", required=True)
parser.add_argument("--conan-dir", required=True)
parser.add_argument("--vcpkg-dir", required=True)

args = parser.parse_args()

package = args.package
repo = args.repo
version = args.version
commit = args.commit

archive = pathlib.Path(args.archive)

if not archive.exists():
    sys.exit(f"Archive not found: {archive}")

data = archive.read_bytes()

# Only vcpkg needs a hash (git itself is the integrity check for the
# Conan git-clone flow; conandata.yml pins a commit, not a tarball).
sha512 = hashlib.sha512(data).hexdigest()

print(f"Version : {version}")
print(f"Commit  : {commit}")
print(f"SHA512  : {sha512}")

# ---------------------------------------------------------------------
# Root CMakeLists.txt
# ---------------------------------------------------------------------

root_cmakelists = pathlib.Path(args.root_dir) / "CMakeLists.txt"

text = root_cmakelists.read_text(encoding="utf-8")


def _bump_project_version(match: re.Match) -> str:
    # Substitute only within the matched project(...) call, so this
    # can't touch an unrelated VERSION elsewhere in the file (e.g.
    # cmake_minimum_required(VERSION ...)).
    return re.sub(
        r"VERSION\s+[0-9A-Za-z.\-_]+",
        f"VERSION {version}",
        match.group(0),
        count=1,
    )


new_text, count = re.subn(
    r"project\s*\([^)]*\)",
    _bump_project_version,
    text,
    count=1,
    flags=re.DOTALL,
)

if count == 0:
    sys.exit(f"No project() call found in {root_cmakelists}")

root_cmakelists.write_text(new_text, encoding="utf-8")

print("✓ Updated CMakeLists.txt")

# ---------------------------------------------------------------------
# Conan
# ---------------------------------------------------------------------

recipe_dir = pathlib.Path(args.conan_dir)

# conanfile.py

conanfile = recipe_dir / "conanfile.py"

text = conanfile.read_text(encoding="utf-8")

text = re.sub(
    r'version\s*=\s*"[^"]+"',
    f'version = "{version}"',
    text,
)

conanfile.write_text(text, encoding="utf-8")

print("✓ Updated conanfile.py")

# conandata.yml

conandata = recipe_dir / "conandata.yml"

# Matches the git-clone flow in conanfile.py's source() (Git(self).run(...)
# against sources['url'] / sources['commit']) -- not a tarball download, so
# no hash is stored here. The commit is the integrity check.
conandata.write_text(
f'''sources:
  "{version}":
    url: "https://github.com/{repo}.git"
    commit: "{commit}"
''',
encoding="utf-8"
)

print("✓ Updated conandata.yml")

# ---------------------------------------------------------------------
# vcpkg
# ---------------------------------------------------------------------

port_dir = pathlib.Path(args.vcpkg_dir)

# portfile.cmake

portfile = port_dir / "portfile.cmake"

text = portfile.read_text(encoding="utf-8")

# Only touches the first REF -- the top-level vcpkg_from_github() block
# for the library itself. SUBMODULE_SPECS entries are plain "dnspro|ref|sha"
# strings (no literal "REF" keyword), and the per-submodule
# vcpkg_from_github() call inside the foreach loop uses REF ${SUBMODULE_REF}
# (a variable, not a hex literal), so neither is matched by this pattern --
# SUBMODULE_SPECS entries are updated separately below, by name, from
# --submodule arguments.
text, ref_count = re.subn(
    r"REF\s+(?:<commit-sha>|[0-9a-fA-F]{7,40})",
    f"REF {commit}",
    text,
    count=1,
)

text, sha_count = re.subn(
    r"SHA512\s+(?:<sha512>|[0-9a-fA-F]+)",
    f"SHA512 {sha512}",
    text,
    count=1,
)

if ref_count == 0 or sha_count == 0:
    sys.exit(
        f"Could not find the top-level vcpkg_from_github() block's "
        f"REF/SHA512 in {portfile} -- expected either the "
        "<commit-sha>/<sha512> placeholders or a real hex value there."
    )

# SUBMODULE_SPECS entries, e.g. "ArenaAllocator|<commit-sha>|<sha512>" or
# "ArenaAllocator|2e58e18...|8ed246af...". Each manifest entry updates
# exactly the SUBMODULE_SPECS line for that name, leaving its neighbors
# untouched. Manifest is optional -- no file, or an empty list, means
# nothing here to update (e.g. a project with no vendored submodules).
submodules = []

if args.submodules_manifest:
    manifest_path = pathlib.Path(args.submodules_manifest)

    if manifest_path.exists():
        submodules = json.loads(manifest_path.read_text(encoding="utf-8"))

for entry in submodules:
    sub_name = entry["name"]
    sub_commit = entry["commit"]
    sub_archive = pathlib.Path(entry["archive"])

    if not sub_archive.exists():
        sys.exit(f"Submodule archive not found for {sub_name}: {sub_archive}")

    sub_sha512 = hashlib.sha512(sub_archive.read_bytes()).hexdigest()

    print(f"Submodule {sub_name}")
    print(f"  Commit  : {sub_commit}")
    print(f"  SHA512  : {sub_sha512}")

    text, sub_count = re.subn(
        rf'"{re.escape(sub_name)}\|[^|]*\|[^"]*"',
        f'"{sub_name}|{sub_commit}|{sub_sha512}"',
        text,
        count=1,
    )

    if sub_count == 0:
        sys.exit(
            f"Could not find a SUBMODULE_SPECS entry for '{sub_name}' in "
            f"{portfile} -- add one there first (see portfile.cmake's own "
            "comment on SUBMODULE_SPECS for the format)."
        )

portfile.write_text(text, encoding="utf-8")

print("✓ Updated portfile.cmake")

# vcpkg.json

vcpkg_json = port_dir / "vcpkg.json"

data = json.loads(vcpkg_json.read_text(encoding="utf-8"))

data["version"] = version

vcpkg_json.write_text(
    json.dumps(data, indent=2) + "\n",
    encoding="utf-8",
)

print("✓ Updated vcpkg.json")