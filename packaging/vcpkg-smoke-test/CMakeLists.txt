cmake_minimum_required(VERSION 3.20)
project(smoke_test CXX)

# Retargeting: the one dnspro to update per project — must match
# PACKAGE_NAME in ../vcpkg/ports/dnspro/portfile.cmake.
set(TESTED_PACKAGE_NAME DnsPro)

find_package(${TESTED_PACKAGE_NAME} REQUIRED CONFIG)

add_executable(smoke_test src/smoke_test.cpp)

target_link_libraries(smoke_test PRIVATE ${TESTED_PACKAGE_NAME}::${TESTED_PACKAGE_NAME})
target_compile_features(smoke_test PRIVATE cxx_std_23)