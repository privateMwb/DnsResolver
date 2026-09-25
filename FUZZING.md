# Fuzzing

DnsPro is fuzzed via [ClusterFuzzLite](https://google.github.io/clusterfuzzlite/),
running on every pull request that touches the fuzzed files, plus a
longer scheduled batch run every night.

## What's covered

**`fuzz_parser.cpp`** is a round-trip fuzzer for `Parser::parse()` and
`Builder::build()`. There's no reference DNS implementation to
differentially compare against (there's no standard-library or
widely-used DNS parser/builder to pair each run with), so instead it
checks the one correctness property the library actually promises for
these two: **a rebuilt packet re-parses to an equivalent `Message`**
(see the "Builder" section of the README, and `Builder.h`'s own doc
comment).

Concretely, for any input that parses successfully:

```
buffer --parse--> message --build--> rebuilt
rebuilt --parse--> reparsed --build--> rebuiltAgain
```

`rebuilt` is not expected to equal the original `buffer` byte-for-byte —
`Builder` never emits compression pointers, so a compressed input can
rebuild to a larger, uncompressed encoding. What *is* expected is a
fixed point one hop later: `rebuiltAgain` must equal `rebuilt` exactly.
A mismatch means `Parser` and `Builder` disagree with each other about
what some `Message` means.

Specifically exercised:

- Every failure path a malformed packet can take before reaching that
  point: truncated buffers (`Status::BUFFER_TOO_SMALL`), invalid label
  encodings (`Status::MALFORMED_NAME`), and mismatched section counts
  (`Status::SECTION_COUNT_MISMATCH`).
- **Compression-pointer loops and forward pointers**
  (`Status::COMPRESSION_LOOP`, `Status::COMPRESSION_FORWARD_POINTER`) —
  the trickiest part of RFC 1035 to get right. `Parser` bounds total
  pointer hops with a step counter specifically to terminate on the
  oscillating case (a label that advances the cursor forward, followed
  by a pointer back to it) that a naive "each hop must be backward"
  check would miss. Under a fuzzer, getting this wrong shows up as a
  hang/timeout, not a clean rejection — exactly the class of bug this
  harness is positioned to catch.
- Every wire-format structure `Parser`/`Builder` round-trip through:
  `Header`, `Name` (including multi-label and compressed names),
  `Question`, and `ResourceRecord` across all four sections.

Built and run under both AddressSanitizer and UndefinedBehaviorSanitizer.

## What's deliberately NOT covered yet

- **`ZoneStore`.** Its `addRecord()`/`lookup()`/`removeRecord()` API
  takes already-parsed `Packet::ResourceRecord`/`Packet::Name` values,
  not raw bytes, so it doesn't fit this harness's byte-buffer-in model
  directly. A follow-up harness would need to fuzz sequences of
  ZoneStore operations against a shadow model (similar in spirit to
  VectorPro's `fuzz_vector.cpp`), not a round-trip check.
- **`Resolver::resolve()` end-to-end.** This would chain
  `fuzz_parser.cpp`'s input through a populated `ZoneStore` and check
  the resulting response is itself a well-formed message Parser can
  re-parse — a natural next harness, but a different one, since it
  needs a zone fixture as well as fuzzer input.
- **Exception-safety / allocation-failure paths.** This harness never
  forces an allocation to fail, so it doesn't exercise whatever
  exception-safety guarantees `Vector`'s reallocation provides when
  called from `Parser`/`Builder`.

## Running locally

```bash
git clone --recursive https://github.com/google/oss-fuzz.git
cd oss-fuzz
python infra/helper.py build_fuzzers --sanitizer address DnsResolver /path/to/DnsResolver
python infra/helper.py run_fuzzer DnsResolver fuzz_parser
```

Or, without OSS-Fuzz's tooling, directly with clang (note the
`--recursive` clone — DnsPro vendors VectorPro/HashMapPro as git
submodules, unlike a dependency-free header-only library):

```bash
git clone --recursive https://github.com/privateMwb/DnsResolver.git
cd DnsResolver

clang++ -std=c++23 -fsanitize=fuzzer,address \
  -Iinclude \
  -Ilibs/internal/VectorPro/include \
  -Ilibs/internal/HashMapPro/include \
  src/DnsPro/Parser.cpp \
  src/DnsPro/Builder.cpp \
  fuzz/fuzz_parser.cpp \
  -o fuzz_parser

./fuzz_parser
```

Add `-fsanitize=fuzzer,undefined` instead to run under UBSan.

## Reproducing a crash

ClusterFuzzLite uploads the failing input as a workflow artifact when
a run fails. Download it, then:

```bash
./fuzz_parser path/to/crash-<hash>
```

This replays that exact byte sequence through
`LLVMFuzzerTestOneInput()` once, deterministically — no sanitizer flags
needed beyond however the binary was already built.

## Adding a new harness

1. Add `fuzz/fuzz_<target>.cpp` with an `extern "C" int
   LLVMFuzzerTestOneInput(const uint8_t*, size_t)` entry point.
2. Add the matching compile + link block to `.clusterfuzzlite/build.sh`,
   including any additional `src/DnsPro/*.cpp` translation units the
   new harness needs (e.g. `ZoneStore.cpp`, `Resolver.cpp`).
3. No workflow changes needed — `cflite_pr.yml`/`cflite_batch.yml`
   build and run every binary `build.sh` produces in `$OUT`. Update
   `cflite_pr.yml`'s `paths:` filter if the new harness touches files
   outside what's already listed there.
