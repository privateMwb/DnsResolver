# ── Retargeting this port for a new library ─────────────────────
# CMAKE_PROJECT_NAME must match project()'s name in the repo's root
# CMakeLists.txt exactly, casing included (e.g. "FalconHTTP") -- NOT
# ${PORT}, which vcpkg forces to lowercase-with-hyphens (e.g.
# "falconhttp") and is a different string. GITHUB_REPO_NAME must match
# the real GitHub repo name, casing included -- also not necessarily
# the same as ${PORT}.
set(CMAKE_PROJECT_NAME DnsPro)
set(GITHUB_REPO_NAME DnsResolver)
# ──────────────────────────────────────────────────────────────

# Reads GITHUB_TOKEN from the environment if present (set by
# packaging.yml's vcpkg job) so these repos, which are private, can be
# fetched -- vcpkg_from_github's own HTTPS downloader has no other way
# to authenticate. Empty string is fine for public consumers/local
# testing: vcpkg treats an empty AUTHORIZATION_TOKEN as "no auth."
if(DEFINED ENV{GITHUB_TOKEN})
    set(GITHUB_AUTH_TOKEN "$ENV{GITHUB_TOKEN}")
else()
    set(GITHUB_AUTH_TOKEN "")
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO privateMwb/${GITHUB_REPO_NAME}
    REF 2e58e1883cd18fd3298407e19183ee5e5474a904
    SHA512 815323417a5a57b52bba8d96f644baa162f24d350fb18b212ae4c90f05124a5532cf29b211270caf5d80e8a479b78d6448794242d7a674dfbb48432838203230
    AUTHORIZATION_TOKEN ${GITHUB_AUTH_TOKEN}
)

# GitHub archive tarballs never include submodule content, so this
# project's internal libraries under libs/internal/ are fetched
# separately here, each pinned to the exact commit the submodule
# points at, then copied into place.
#
# Table of name|ref|sha512 -- add a submodule by adding one line here,
# not by copy-pasting a whole vcpkg_from_github() block. The line
# below is a placeholder example; replace with your actual submodules.
set(SUBMODULE_SPECS
    "ArenaAllocator|7fe76fb4c3f1d98edeb34832834baece8481de79|39eceb4db5d18d3a5339da7a761772f956dc13521726605bd3683ad0ce62a2c55cf7ac9c546ce626badfc571a83b2193ed86fcae12020e7d168381a592de5f92"
    "HashMapPro|497b996dfa4721136a4c89edefd36e6a0dfe1d09|3d696060c666b9ea9afa02e9394f637967851cf167d864ec2699858b02370fca52ad8c796552be726d0534373fa828aec0879aaa9ba17d9692b902c199c8eba9"
    "VectorPro|26407a59ecd6fefe69565c980b8de49332e469e8|969c97bfad58f94a75ba35c8723812badad112d5b304e73b555715ad61657055a7c41c6568fadeb5e8a21bca27215435abef28e6a1221ff14f6d2244ab3ca081"
)

foreach(SPEC ${SUBMODULE_SPECS})
    string(REPLACE "|" ";" SPEC_PARTS "${SPEC}")
    list(GET SPEC_PARTS 0 SUBMODULE_NAME)
    list(GET SPEC_PARTS 1 SUBMODULE_REF)
    list(GET SPEC_PARTS 2 SUBMODULE_SHA512)

    file(REMOVE_RECURSE "${SOURCE_PATH}/libs/internal/${SUBMODULE_NAME}")

    vcpkg_from_github(
        OUT_SOURCE_PATH SUBMODULE_SOURCE_PATH
        REPO privateMwb/${SUBMODULE_NAME}
        REF ${SUBMODULE_REF}
        SHA512 ${SUBMODULE_SHA512}
        AUTHORIZATION_TOKEN ${GITHUB_AUTH_TOKEN}
    )

    file(RENAME "${SUBMODULE_SOURCE_PATH}" "${SOURCE_PATH}/libs/internal/${SUBMODULE_NAME}")
endforeach()

# Consumers only need the library itself, not the tests, benchmarks,
# regression tools, or examples.
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTS=OFF
        -DBUILD_BENCHMARKS=OFF
        -DBUILD_REGRESSION=OFF
        -DBUILD_EXAMPLES=OFF
)

vcpkg_cmake_install()

# PACKAGE_NAME/CONFIG_PATH must match CMAKE_PROJECT_NAME above, not
# ${PORT}: this points at wherever the library's own root
# CMakeLists.txt called install(EXPORT ... DESTINATION
# lib/cmake/${PROJECT_NAME}), and that PROJECT_NAME keeps whatever
# casing project() used -- vcpkg's forced-lowercase ${PORT} will not
# match it on a case-sensitive filesystem.
vcpkg_cmake_config_fixup(
    PACKAGE_NAME ${CMAKE_PROJECT_NAME}
    CONFIG_PATH lib/cmake/${CMAKE_PROJECT_NAME}
)

# This library is compiled (not header-only), so debug binaries are
# real and must be kept -- only the duplicate debug/include headers
# are removed.
file(
    REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
)

vcpkg_install_copyright(
    FILE_LIST "${SOURCE_PATH}/LICENSE"
)
