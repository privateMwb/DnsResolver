# Test Suite

This document describes the test categories under `suite/` — what each
one verifies, and the individual test files it contains.

Unlike the benchmark suite, tests validate the library's own
correctness directly — there is no reference implementation to compare
against, so results are simply pass or fail.

Every test suite registers itself automatically via
`REGISTER_TEST_SUITE()` at startup, and is assigned a sequential id
within its category (e.g. `U1`, `U2` for Unit; `L1`, `L2` for
Lifecycle) — there's no suite list to maintain by hand. This applies
uniformly across every category below.

---

## Concurrency

Verifies thread-safety — concurrent reads and writes from multiple
threads, and correctness under simultaneous access.

### Tests

- `resolver_concurrent_resolve.cpp` — concurrent `resolve()` reads
  against a shared, already-populated `ZoneStore` each produce a
  correct, independent answer: `concurrent_resolve_reads`
- `zonestore_concurrent_access.cpp` — concurrent `addRecord()` calls
  under distinct names all land, with the correct final `recordCount()`:
  `concurrent_add_record`

---

## Integration

Verifies multiple components working together end-to-end — for
example, a full parse-modify-serialize round trip — rather than a
single function in isolation.

### Tests

- `compression_pointer_chain.cpp` — a two-hop compression pointer
  chain resolves to the correct name: `two_hop_pointer_chain_resolves`
- `empty_zone_query.cpp` — resolving against a zero-record `ZoneStore`
  returns a well-formed NXDOMAIN response: `empty_zone_returns_nxdomain`
- `malformed_query_handling.cpp` — a query `Parser::parse()` can't
  parse produces the same `Status` through `Resolver::resolve()` as
  parsing it directly would: `truncated_query_status_propagates`
- `parse_build_roundtrip.cpp` — a fully-populated `Message` (question,
  answer, authority, additional) survives `Builder::build()` then
  `Parser::parse()` intact: `full_message_roundtrip`
- `resolve_roundtrip.cpp` — raw query bytes through `Resolver::resolve()`
  produce a response that re-parses correctly, for both the hit and
  NXDOMAIN cases: `resolve_roundtrip_hit`, `resolve_roundtrip_nxdomain`
- `multi_question_query.cpp` — a multi-question query only answers the
  first question, per `Resolver::resolve()`'s documented behavior:
  `multi_question_answers_first_only`

---

## Lifecycle

Verifies object lifetime operations — construction, destruction,
copying, and moving — across the different states or value kinds the
library's core type can hold.

### Tests

- `message_move.cpp` — `Message` move-construct and move-assign both
  transfer data and leave the source empty: `move_construct_transfers_data`,
  `move_assign_transfers_data`
- `resolver_zone_reference.cpp` — `Resolver` holds a reference to its
  `ZoneStore`, not a copy: `resolver_sees_later_zone_changes`
- `zonestore_copy.cpp` — a copied `ZoneStore` is fully independent of
  its source: `copy_is_independent`

---

## Regression

Verifies that a specific, previously fixed bug stays fixed. One test
per resolved issue, added at the time the fix lands.

### Tests

None yet — populated as fixes land.

---

## Unit

Verifies individual functions or methods in isolation — the smallest
testable unit of behavior, independent of the categories above.

### Tests

- `zonestore.cpp` — name case-insensitivity, type filtering on
  `lookup()`/`contains()`, `removeRecord()` hit/miss, and bucket
  cleanup after removing a name's last record:
  `case_insensitive_name_matching`, `type_filtering_lookup_contains`,
  `remove_record_hit_and_miss`, `remove_record_bucket_cleanup`
- `buffer_too_small.cpp` — truncated buffers are rejected at each
  stage (header, question name, question fields, rdata):
  `truncated_header_rejected`, `truncated_question_name_rejected`,
  `truncated_question_fields_rejected`, `truncated_rdata_rejected`
- `build_limits.cpp` — label length, section size, and rdata size
  limits are enforced on build: `oversized_label_rejected`,
  `boundary_label_accepted`, `oversized_section_rejected`,
  `oversized_rdata_rejected`
- `compression.cpp` — forward compression pointers are rejected, valid
  backward pointers are accepted: `forward_pointer_rejected`,
  `far_forward_pointer_rejected`, `valid_backward_pointer_accepted`
- `header_flags_roundtrip.cpp` — header flag bits survive
  `Builder::build()` then `Parser::parse()` unchanged, at both extremes:
  `all_zero_flags_roundtrip`, `all_flags_set_roundtrip`
- `rdata_name_compression.cpp` — `ResourceRecord::rdataOffset` lands
  exactly on rdata's first byte, and the public `Parser::parseName()`,
  called externally at that offset against the original message buffer,
  correctly decodes a compressed domain name embedded inside rdata (an
  MX record's exchange, part literal label and part compression
  pointer): `rdata_offset_lands_on_rdata_start`,
  `parse_name_decodes_compressed_exchange`
