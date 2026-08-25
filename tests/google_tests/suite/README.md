# Test Suite

This document describes the test categories under `suite/` — what each
one verifies, and the individual test files it contains.

Unlike the benchmark suite, tests validate the library's own
correctness directly — there is no reference implementation to compare
against, so results are simply pass or fail.

Every test registers itself automatically via GoogleTest's `TEST()`
macro at startup — there's no suite list to maintain by hand. Each
test is named `Category.TestName` (e.g. `Unit.BasicBehavior`,
`Lifecycle.MoveValidity`), where `Category` matches the section below
and doubles as the filter you'd pass to `--gtest_filter` or `ctest -R`.
This applies uniformly across every category below.

---

## Concurrency

Verifies thread-safety — concurrent reads and writes from multiple
threads, and correctness under simultaneous access.

### Tests

- `resolver_concurrent_resolve.cpp` — concurrent `resolve()` reads
  against a shared, already-populated `ZoneStore` each produce a
  correct, independent answer: `ResolverConcurrentResolve.ConcurrentResolveReads`
- `zonestore_concurrent_access.cpp` — concurrent `addRecord()` calls
  under distinct names all land, with the correct final `recordCount()`:
  `ZoneStoreConcurrentAccess.ConcurrentAddRecord`

---

## Integration

Verifies multiple components working together end-to-end — for
example, a full parse-modify-serialize round trip — rather than a
single function in isolation.

### Tests

- `compression_pointer_chain.cpp` — a two-hop compression pointer
  chain resolves to the correct name:
  `CompressionPointerChain.TwoHopPointerChainResolves`
- `empty_zone_query.cpp` — resolving against a zero-record `ZoneStore`
  returns a well-formed NXDOMAIN response:
  `EmptyZoneQuery.EmptyZoneReturnsNxdomain`
- `malformed_query_handling.cpp` — a query `Parser::parse()` can't
  parse produces the same `Status` through `Resolver::resolve()` as
  parsing it directly would: `MalformedQueryHandling.TruncatedQueryStatusPropagates`
- `parse_build_roundtrip.cpp` — a fully-populated `Message` (question,
  answer, authority, additional) survives `Builder::build()` then
  `Parser::parse()` intact: `ParseBuildRoundtrip.FullMessageRoundtrip`
- `resolve_roundtrip.cpp` — raw query bytes through `Resolver::resolve()`
  produce a response that re-parses correctly, for both the hit and
  NXDOMAIN cases: `ResolveRoundtrip.ResolveRoundtripHit`,
  `ResolveRoundtrip.ResolveRoundtripNxdomain`
- `multi_question_query.cpp` — a multi-question query only answers the
  first question, per `Resolver::resolve()`'s documented behavior:
  `MultiQuestionQuery.MultiQuestionAnswersFirstOnly`

---

## Lifecycle

Verifies object lifetime operations — construction, destruction,
copying, and moving — across the different states or value kinds the
library's core type can hold.

### Tests

- `message_move.cpp` — `Message` move-construct and move-assign both
  transfer data and leave the source empty:
  `MessageMove.MoveConstructTransfersData`, `MessageMove.MoveAssignTransfersData`
- `resolver_zone_reference.cpp` — `Resolver` holds a reference to its
  `ZoneStore`, not a copy: `ResolverZoneReference.ResolverSeesLaterZoneChanges`
- `zonestore_copy.cpp` — a copied `ZoneStore` is fully independent of
  its source: `ZoneStoreCopy.CopyIsIndependent`

---

## Regression

Verifies that a specific, previously fixed bug stays fixed. One test
per resolved issue, added at the time the fix lands.

### Tests

None yet — this category is populated as fixes land, not converted
from the custom suite (it had none to carry over either).

---

## Unit

Verifies individual functions or methods in isolation — the smallest
testable unit of behavior, independent of the categories above.

### Tests

- `zonestore.cpp` — name case-insensitivity, type filtering on
  `lookup()`/`contains()`, `removeRecord()` hit/miss, and bucket
  cleanup after removing a name's last record:
  `ZoneStore.CaseInsensitiveNameMatching`, `ZoneStore.TypeFilteringLookupContains`,
  `ZoneStore.RemoveRecordHitAndMiss`, `ZoneStore.RemoveRecordBucketCleanup`
- `buffer_too_small.cpp` — truncated buffers are rejected at each
  stage (header, question name, question fields, rdata):
  `BufferTooSmall.TruncatedHeaderRejected`, `BufferTooSmall.TruncatedQuestionNameRejected`,
  `BufferTooSmall.TruncatedQuestionFieldsRejected`, `BufferTooSmall.TruncatedRdataRejected`
- `build_limits.cpp` — label length, section size, and rdata size
  limits are enforced on build: `BuildLimits.OversizedLabelRejected`,
  `BuildLimits.BoundaryLabelAccepted`, `BuildLimits.OversizedSectionRejected`,
  `BuildLimits.OversizedRdataRejected`
- `compression.cpp` — forward compression pointers are rejected, valid
  backward pointers are accepted: `Compression.ForwardPointerRejected`,
  `Compression.FarForwardPointerRejected`, `Compression.ValidBackwardPointerAccepted`
- `header_flags_roundtrip.cpp` — header flag bits survive
  `Builder::build()` then `Parser::parse()` unchanged, at both extremes:
  `HeaderFlagsRoundtrip.AllZeroFlagsRoundtrip`, `HeaderFlagsRoundtrip.AllFlagsSetRoundtrip`
