# Google Benchmarks

This document describes the benchmark categories under `suite/` — what
each one measures, and the individual benchmarks it contains. Same
categories as `../custom/suite/`, reimplemented with Google Benchmark
instead of the custom framework.

No comparison baseline exists for DnsResolver — there's no reference
implementation to benchmark it against, so unlike a typical Google
Benchmark suite with paired library-vs-reference functions, every
benchmark here is a single `BENCHMARK()` (or `BENCHMARK_CAPTURE()`)
function timing DnsResolver alone. This applies uniformly across the
whole suite; it is not specific to any one category.

Iteration count is handled by Google Benchmark itself — each
`BENCHMARK()` runs until `--benchmark_min_time` is satisfied, the
Google Benchmark equivalent of the custom suite's SMALL/MEDIUM/LARGE
tiers, without needing to register separate sizes by hand. The
**Scaling** category below measures something different: how
per-operation cost changes as capacity itself grows or shrinks,
independent of iteration count — those benchmarks use
`BENCHMARK_CAPTURE(...)` to register one function per structural size
instead.

Every benchmark auto-registers via `BENCHMARK(...)` or
`BENCHMARK_CAPTURE(...)` at startup — no suite list to maintain by
hand. Benchmark names double as the filter you'd pass to
`--benchmark_filter`, e.g. `--benchmark_filter=Lookup` runs everything
with "Lookup" in its name. This applies uniformly across every
category below.

---

## Access

Benchmarks read and lookup operations against an already-populated
ZoneStore.

### Benchmarks

- `lookup.cpp` — `ZoneStore::lookup()` across all three outcomes:
  `Lookup_ExistingNameType` (match found), `Lookup_MissingName`
  (NXDOMAIN), `Lookup_ExistingNameMissingType` (NODATA)

---

## Core

Benchmarks the fundamental, most frequently exercised operations —
parsing a query, building a response, writing into the zone store, and
the full resolve pipeline that ties them together.

### Benchmarks

- `parse.cpp` — `Parser::parse()`: `Parse_QuestionOnly`,
  `Parse_FourAnswerRecords`
- `build.cpp` — `Builder::build()` on the same two message shapes:
  `Build_QuestionOnly`, `Build_FourAnswerRecords`
- `record.cpp` — `ZoneStore::addRecord()` and `removeRecord()`:
  `AddRecord_ExistingBucket`, `RemoveRecord_MissingName`
- `resolve.cpp` — full `Resolver::resolve()` pipeline across all three
  outcomes: `Resolve_AnswerFound`, `Resolve_NXDOMAIN`, `Resolve_NODATA`

---

## Lifecycle

Benchmarks object lifetime operations — construction, destruction, and
moving. Thinner here than in FalconHTTP: DnsResolver doesn't wrap any
native OS resources (no sockets, no server), so there's no
`socket_construction.cpp`/`server_construction.cpp` equivalent —
`Message` move is the one lifetime cost worth isolating.

### Benchmarks

- `message_move.cpp` — `Message` move-construct and move-assign:
  `Message_MoveConstruct`, `Message_MoveAssign`

---

## Scaling

Benchmarks how per-operation cost changes as a structural size grows —
a separate axis from Google Benchmark's own iteration-count tuning
above: that repeats the same fixed-size operation until timing
stabilizes, while Scaling grows the structure itself (answer count,
zone record count, name depth) via `BENCHMARK_CAPTURE(...)` and
observes the resulting per-call cost.

### Benchmarks

- `answer_count_growth.cpp` — parse/build cost as `ancount` grows:
  `ParseAt`/`BuildAt` captured at `FourAnswerRecords`,
  `SixteenAnswerRecords`, `SixtyFourAnswerRecords`
- `zone_size_growth.cpp` — `lookup()` cost as ZoneStore record count
  grows: `LookupAt` captured at `Names100`, `Names1000`, `Names10000`
- `label_depth_growth.cpp` — name parse/build cost as label count
  grows: `ParseAt`/`BuildAt` captured at `TwoLabelsDeep`,
  `EightLabelsDeep`, `ThirtyTwoLabelsDeep`

---

## Utility

Benchmarks small, frequently-called conversion and lookup functions
that don't belong to any of the categories above — the isolated name
encode/decode steps, and zone-key canonicalization.

### Benchmarks

- `name_parse.cpp` — name parsing, uncompressed vs. via a compression
  pointer: `NameParse_Uncompressed`, `NameParse_Compressed`
- `canonicalize.cpp` — `ZoneStore`'s name -> canonical string key
  conversion, via `addRecord()`: `Canonicalize_MixedCaseName`
