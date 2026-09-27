# Benchmark Suite

This document describes the benchmark categories under `suite/` — what each
one measures, and the individual benchmarks it contains.

| Category | Focus |
|---|---|
| [Access](#access) | Read and lookup operations against an already-populated `ZoneStore` |
| [Core](#core) | The fundamental, most frequently exercised operations — parsing, building, record mutation, and the full resolve pipeline |
| [Lifecycle](#lifecycle) | Object lifetime operations — `Message` move construction and assignment |
| [Scaling](#scaling) | Cost vs. structural size (answer count, zone record count, name depth), independent of iteration count |
| [Utility](#utility) | Small, frequently-called conversion and lookup functions |
| [Conventions](#conventions) | Registration, comparison, and sizing conventions specific to the custom framework |

Every benchmark compares the library's core type against a reference
implementation — the conventional way the same functionality is typically
built without this library. A category can support more than one reference
for comparison, but for now each category is benchmarked against a single
reference.

Every `BENCH()` call, in every category below, is automatically repeated at
three iteration tiers — SMALL (10K), MEDIUM (100K), and LARGE (1M) — to
smooth out timing noise and show whether relative performance holds steady
as call volume increases. This applies uniformly across the whole suite; it
is not specific to any one category. The **Scaling** category below measures
something different: how per-operation cost changes as capacity itself
grows or shrinks, independent of iteration count.

Some benchmarks have no meaningful reference equivalent — the reference
implementation may lack certain introspection or statistics the library's
type provides. Those run through `BENCH_SOLO()` instead of `BENCH()`, timing
the library's type alone.

---

## Access

Benchmarks read and lookup operations against an already-populated
`ZoneStore`.

### Benchmarks

| File | What it covers |
|---|---|
| `lookup.cpp` | `ZoneStore::lookup()` across all three outcomes: match found, name doesn't exist (NXDOMAIN), name exists but wrong type (NODATA) |

---

## Core

Benchmarks the fundamental, most frequently exercised operations — parsing
a query, building a response, writing into the zone store, and the full
resolve pipeline that ties them together.

### Benchmarks

| File | What it covers |
|---|---|
| `parse.cpp` | `Parser::parse()` on a question-only query and a response with answer records |
| `build.cpp` | `Builder::build()` on the same two message shapes |
| `record.cpp` | `ZoneStore::addRecord()` and `removeRecord()` |
| `resolve.cpp` | Full `Resolver::resolve()` pipeline across all three outcomes: answer found, NXDOMAIN, NODATA |

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
| `message_move.cpp` | `Message` move-construct and move-assign |

---

## Scaling

Benchmarks how per-operation cost changes as a structural size grows — a
separate axis from the SMALL/MEDIUM/LARGE iteration tiers above: those
repeat the same fixed-size operation more times, while Scaling grows the
structure itself (answer count, zone record count, name depth) and observes
the resulting per-call cost.

### Benchmarks

| File | What it covers |
|---|---|
| `answer_count_growth.cpp` | Parse/build cost as `ancount` grows |
| `zone_size_growth.cpp` | `lookup()` cost as `ZoneStore` record count grows |
| `label_depth_growth.cpp` | Name parse/build cost as label count grows |

---

## Utility

Benchmarks small, frequently-called conversion and lookup functions that
don't belong to any of the categories above — the isolated name
encode/decode steps, and zone-key canonicalization.

### Benchmarks

| File | What it covers |
|---|---|
| `name_parse.cpp` | Name parsing, uncompressed vs. via a compression pointer |
| `canonicalize.cpp` | `ZoneStore`'s name -> canonical string key conversion, via `addRecord()` |

---

## Conventions

- **Registration** — every case is a `static void bench_<n>()` called from
  the file's `run_benchmarks()`, and the file ends with
  `REGISTER_BENCH_SUITE();`.
- **Comparison form** — `BENCH("label", referenceImpl, libraryImpl)` times
  the reference and the library implementation side by side; `BENCH_SOLO
  ("label", libraryImpl)` times the library implementation alone, for cases
  with no meaningful reference equivalent.
- **Matching work** — a `BENCH()` pair only compares fairly if both sides do
  equivalent work on equivalent input; when the reference can't be made to
  do exactly the same thing (e.g. it lacks an outcome the library
  distinguishes), that case moves to `BENCH_SOLO()` rather than being forced
  into an uneven comparison.
- **Preventing elision** — every lambda's result is consumed (e.g. via a
  `(void)` cast or by writing it somewhere observable) so the call can't be
  optimized away.
- **Iteration tiers, not structural growth** — SMALL/MEDIUM/LARGE (10K/
  100K/1M) repeat the same fixed-size operation more times to smooth out
  timing noise; they are not a substitute for the Scaling category, which
  grows the structure itself and observes the resulting per-call cost.
