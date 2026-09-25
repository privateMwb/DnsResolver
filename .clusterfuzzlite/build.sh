#!/bin/bash -eu
# ============================================================
# .clusterfuzzlite/build.sh
#
# Unlike a header-only library, DnsPro has real translation units under
# src/DnsPro/ that need compiling alongside the fuzz target itself, plus
# two vendored submodules (VectorPro, HashMapPro) under libs/internal/
# to put on the include path. Only Parser.cpp and Builder.cpp are
# compiled in here -- that's all fuzz_parser.cpp exercises; ZoneStore.cpp
# and Resolver.cpp join this list once a harness actually calls them.
#
# Add more `${SRC}/DnsResolver/fuzz/fuzz_*.cpp` harnesses here as
# they're added; each becomes its own $OUT binary.
# ============================================================

cd "${SRC}/DnsResolver"

$CXX $CXXFLAGS -std=c++23 \
  -I"${SRC}/DnsResolver/include" \
  -I"${SRC}/DnsResolver/libs/internal/VectorPro/include" \
  -I"${SRC}/DnsResolver/libs/internal/HashMapPro/include" \
  src/DnsPro/Parser.cpp \
  src/DnsPro/Builder.cpp \
  fuzz/fuzz_parser.cpp \
  $LIB_FUZZING_ENGINE \
  -o "${OUT}/fuzz_parser"
