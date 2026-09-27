# Google Benchmark Suite

This document describes the benchmark categories under `suite/` — what each
one measures, and the individual benchmarks it contains. Same categories as
`../custom/suite/`, reimplemented with Google Benchmark instead of the
custom framework.

| Category | Focus |
|---|---|
| [Access](#access) | Read and lookup operations against an already-populated `ZoneStore` |
| [Core](#core) | The fundamental, most frequently exercised operations — parsing, building, record mutation, and the full resolve pipeline |
| [Lifecycle](#lifecycle) | Object lifetime operations — `Message` move construction and assignment |
| [Scaling](#scaling) | Cost vs. structural size (answer count, zone record count, name depth), independent of iteration count |
| [Utility](#utility) | Small, frequently-called conversion and lookup functions |
| [Conventions](#conventions) | Registration, naming, and sizing conventions specific to Google Benchmark |

No comparison baseline exists for DnsResolver — there's no reference
implementation to benchmark it against, so unlike a typical Google
Benchmark suite with paired library-vs-reference functions, every benchmark
here is a single `BENCHMARK()` (or `BENCHMARK_CAPTURE()`) function timing
DnsResolver alone. This applies uniformly across the whole suite; it is not
specific to any one category.

Iteration count is handled by Google Benchmark itself — each `BENCHMARK()`
runs until `--benchmark_min_time` is satisfied, the Google Benchmark
equivalent of the custom suite's SMALL/MEDIUM/LARGE tiers, without needing
to register separate sizes by hand. The **Scaling** category below measures
something different: how per-operation cost changes as capacity itself
grows or shrinks, independent of iteration count — those benchmarks use
`BENCHMARK_CAPTURE(...)` to register one function per structural size
instead.

Every benchmark auto-registers via `BENCHMARK(...)` or
`BENCHMARK_CAPTURE(...)` at startup — no suite list to maintain by hand.
Benchmark names double as the filter you'd pass to `--benchmark_filter`,
e.g. `--benchmark_filter=Lookup` runs everything with "Lookup" in its name.
This applies uniformly across every category below.

---

## Access

Benchmarks read and lookup operations against an already-populated
`ZoneStore`.

### Benchmarks

| File | What it covers |
|---|---|
| `lookup.cpp` | `ZoneStore::lookup()` across all three outcomes: `Lookup_ExistingNameType` (match found), `Lookup_MissingName` (NXDOMAIN), `Lookup_ExistingNameMissingType` (NODATA) |

---

## Core

Benchmarks the fundamental, most frequently exercised operations — parsing
a query, building a response, writing into the zone store, and the full
resolve pipeline that ties them together.

### Benchmarks

| File | What it covers |
|---|---|
| `parse.cpp` | `Parser::parse()`: `Parse_QuestionOnly`, `Parse_FourAnswerRecords` |
| `build.cpp` | `Builder::build()` on the same two message shapes: `Build_QuestionOnly`, `Build_FourAnswerRecords` |
| `record.cpp` | `ZoneStore::addRecord()` and `removeRecord()`: `AddRecord_ExistingBucket`, `RemoveRecord_MissingName` |
| `resolve.cpp` | Full `Resolver::resolve()` pipeline across all three outcomes: `Resolve_AnswerFound`, `Resolve_NXDOMAIN`, `Resolve_NODATA` |

---

## Lifecycle

Benchmarks object lifetime operations — construction, destruction, and
moving. Thinner here than in FalconHTTP: `DnsResolver` doesn't wrap any
native OS resources (no sockets, no server), so there's no
`socket_construction.cpp`/`server_construction.cpp` equivalent — `Message`
move is the one lifetime cost worth isolating.

### Benchmarks

| File | What it covers |
|---|---|
| `message_move.cpp` | `Message` move-construct and move-assign: `Message_MoveConstruct`, `Message_MoveAssign` |

---

## Scaling

Benchmarks how per-operation cost changes as a structural size grows — a
separate axis from Google Benchmark's own iteration-count tuning above:
that repeats the same fixed-size operation until timing stabilizes, while
Scaling grows the structure itself (answer count, zone record count, name
depth) via `BENCHMARK_CAPTURE(...)` and observes the resulting per-call
cost.

### Benchmarks

| File | What it covers |
|---|---|
| `answer_count_growth.cpp` | Parse/build cost as `ancount` grows: `ParseAt`/`BuildAt` captured at `FourAnswerRecords`, `SixteenAnswerRecords`, `SixtyFourAnswerRecords` |
| `zone_size_growth.cpp` | `lookup()` cost as `ZoneStore` record count grows: `LookupAt` captured at `Names100`, `Names1000`, `Names10000` |
| `label_depth_growth.cpp` | Name parse/build cost as label count grows: `ParseAt`/`BuildAt` captured at `TwoLabelsDeep`, `EightLabelsDeep`, `ThirtyTwoLabelsDeep` |

---

## Utility

Benchmarks small, frequently-called conversion and lookup functions that
don't belong to any of the categories above — the isolated name
encode/decode steps, and zone-key canonicalization.

### Benchmarks

| File | What it covers |
|---|---|
| `name_parse.cpp` | Name parsing, uncompressed vs. via a compression pointer: `NameParse_Uncompressed`, `NameParse_Compressed` |
| `canonicalize.cpp` | `ZoneStore`'s name -> canonical string key conversion, via `addRecord()`: `Canonicalize_MixedCaseName` |

---

## Conventions

- **Registration** — every case is a `static void Name(benchmark::State&
  state)` (or one taking `benchmark::State&` plus a captured argument for
  the Scaling category) registered directly below it with `BENCHMARK(Name)`
  or `BENCHMARK_CAPTURE(Name, VariantLabel, arg)`. No file defines `main()`
  or calls `BENCHMARK_MAIN()`.
- **Naming** — a benchmark's registered name doubles as its
  `--benchmark_filter` pattern, so names describe the operation and the
  specific case (`Lookup_MissingName`, `Resolve_NXDOMAIN`) rather than just
  the function under test, letting a single case be filtered to on its own.
- **No baseline to match** — with no reference implementation for
  `DnsResolver`, there's no second implementation to keep in step with;
  every benchmark measures the library alone, so none of this suite uses
  the library-vs-reference pairing a typical Google Benchmark suite would.
- **Iteration count vs. structural size** — a plain `BENCHMARK(Name)` lets
  Google Benchmark pick its own iteration count via `--benchmark_min_time`;
  `BENCHMARK_CAPTURE(Name, VariantLabel, arg)` is reserved for the Scaling
  category, where the axis under test is the structural size passed as
  `arg`, not how many times the loop runs.
- **Preventing elision** — every result that matters is wrapped in
  `benchmark::DoNotOptimize(...)` so it can't be optimized away.
