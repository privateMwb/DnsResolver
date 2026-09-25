# ============================================================
# .clusterfuzzlite/Dockerfile
# Base image for building the fuzz harnesses under ClusterFuzzLite.
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder@sha256:8b4a73d83374b298a0a771eeec9a1d44ceff3416a678b7d7946303389d022aca

COPY . $SRC/DnsResolver
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/DnsResolver

# DnsResolver vendors VectorPro and HashMapPro as git submodules under
# libs/internal/ -- unlike a dependency-free, header-only project, an
# empty checkout of this repo alone isn't enough for build.sh's include
# paths to resolve. If whatever checked out this repo already populated
# submodules, this is a no-op; if not, this fills them in from the
# commit pinned in .gitmodules before build.sh ever runs.
RUN if [ -d .git ]; then git submodule update --init --recursive; fi
