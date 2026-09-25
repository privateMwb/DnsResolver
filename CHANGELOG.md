# Changelog

All notable changes to DnsResolver are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Nothing yet.

## [1.0.0] - 2026-08-25

The first stable release of DnsResolver, a C++23 library for parsing, building,
and resolving DNS messages against an in-memory authoritative zone.

### Added
- RFC 1035 wire-format packet structs — `Header`, `Name`, `Question`,
  `ResourceRecord`, and `Message` — under `DnsPro::Packet`.
- `Parser`: offset-driven, single-pass parsing of bytes into a `Message`,
  with DNS name-compression pointer support (RFC 1035 §4.1.4).
- Compression-pointer loop detection that bounds total hops with a step
  counter, catching forward-jump-then-backward-pointer oscillation, not
  just simple backward jumps (`Status::COMPRESSION_FORWARD_POINTER`,
  `Status::COMPRESSION_LOOP`).
- `Builder`: encodes a `Message` back to bytes, always writing full,
  uncompressed names for a simpler, allocation-light encode path.
- `ZoneStore`: an in-memory, case-insensitive (RFC 4343) authoritative
  record store, indexed `name → type → records` via nested `HashMap`s.
- `Resolver`: ties `Parser` + `ZoneStore` + `Builder` into a single
  `resolve()` call, with correct NOERROR/NXDOMAIN/NODATA handling.
- `Status` enum for explicit, switchable error handling across parse,
  build, and resolve paths.
- Built on `VectorPro` and `HashMapPro`, vendored as git submodules under
  `libs/internal/`.

### Performance
- Nested `name → type → records` indexing in `ZoneStore` keeps `lookup()`
  and `removeRecord()` cost independent of zone size — flat at ~144 ns/op
  from 100 names up to 10,000.
- Zero-copy `Message` move-construct/move-assign at ~6 ns/op regardless of
  question/answer record count, since `Vector`'s move steals the
  underlying buffer pointer.
- `Builder` skips compression entirely, trading wire size for a simpler
  encode path with no back-reference bookkeeping.
- Benchmarked solo at 10K / 100K / 1M iterations (no comparable
  standard-library implementation exists). Full results in
  `benchmarks/baselines/v1.0.0.json`.

### Testing
- Comprehensive test suite — the project's own `custom/` framework and an
  equivalent `google_tests/` suite — covering unit, integration,
  regression, lifecycle, and concurrency tests.
- Concurrency tests deliberately exercise `ZoneStore`'s real
  unsynchronized write race, since it has no internal locking yet.
- 89.2% line coverage (265/297 lines) and 86.2% function coverage
  (25/29 functions), excluding test infrastructure and vendored
  submodules.

### CI
- Automated builds and tests across GCC, Clang, MSVC, and AppleClang,
  each in Debug and Release configurations.
- ASan/UBSan sanitizer job on Linux; `clang-format` and `clang-tidy`
  checks; `lcov`-based coverage collection; CodeQL security analysis; and
  Conan/vcpkg packaging verified against real consumer smoke tests.

[Unreleased]: https://github.com/privateMwb/DnsResolver/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/privateMwb/DnsResolver/releases/tag/v1.0.0
