# Example Suite

This document describes the example categories under `suite/` — what
each one demonstrates, and the individual example files it contains.

Unlike the test suite, an example doesn't assert correctness — it
demonstrates real usage of the library, including deliberate misuse
where instructive (see Misuse), so the reader sees both the correct
pattern and the mistake it guards against.

Every example file ends with `REGISTER_EXAMPLE_SUITE()`, which derives
the suite's category from its containing directory and assigns it a
sequential id within that category. This applies uniformly across
every category below.

---

## Advanced

Demonstrates deeper mechanics of the library — move semantics,
exception safety, and other implementation-level behavior worth
calling out explicitly.

### Examples

- `compression_pointers.cpp` — Parser following an RFC 1035 S4.1.4
  compression pointer, hand-assembled since Builder never emits one.
- `copy_assignment.cpp` — ZoneStore's copy constructor and
  copy-assignment operator producing a fully independent store.
- `move_zone_store.cpp` — ZoneStore's move constructor and
  move-assignment operator transferring ownership of stored records.
- `thread_safe_lookup.cpp` — concurrent reads across multiple threads
  against a single ZoneStore, safe without external synchronization.

---

## Integration

Demonstrates interoperability with the rest of a codebase — custom
types, standard library algorithms, and embedding the library inside a
larger class.

### Examples

- `custom_record_types.cpp` — treating record types as opaque
  `std::uint16_t` values, including RFC types and private-use types the
  library never hardcodes.
- `embed_in_server.cpp` — wrapping ZoneStore and Resolver inside an
  owning class with a simple `handleQuery()` surface.
- `std_algorithm_use.cpp` — using `<algorithm>` (`std::count_if`,
  `std::any_of`) directly over a `lookup()` result.

---

## Misuse

Demonstrates common mistakes and the exceptions or undefined behavior
they lead to, alongside the correct pattern — including examples shown
but not executed, so the reader can see what to avoid without the
program actually invoking undefined behavior.

### Examples

- `forward_pointer.cpp` — a compression pointer targeting an offset at
  or after its own position, rejected as `COMPRESSION_FORWARD_POINTER`.
- `oversized_label.cpp` — a label over the 63-byte encoding limit,
  rejected by `Builder::writeName()` as `LABEL_TOO_LONG`.
- `section_count_mismatch.cpp` — a header claiming more section
  entries than the buffer actually holds; documents that this build
  surfaces it as `BUFFER_TOO_SMALL`, not `SECTION_COUNT_MISMATCH`.
- `unterminated_name.cpp` — a name encoding with no terminating
  zero-length label, caught as `BUFFER_TOO_SMALL` before reading past
  the buffer.

---

## Patterns

Demonstrates common usage idioms built on top of the core API.

### Examples

- `add_and_remove.cpp` — the add/verify/remove/verify cycle for
  ZoneStore records, including bucket pruning on the last removal.
- `authoritative_lookup.cpp` — the three outcomes an authoritative
  resolve() call distinguishes: an answer, NXDOMAIN, and NODATA.
- `multi_type_records.cpp` — one name holding records of several types
  at once, looked up independently per type.
- `round_trip_message.cpp` — building a Message to bytes and parsing
  it back, then comparing the result field-by-field.

---

## Quickstart

Demonstrates fundamental, everyday usage — construction, and the
basic operations most callers will reach for first.

### Examples

- `build_response.cpp` — constructing an authoritative response
  Message by hand and serializing it with `Builder::build()`.
- `parse_message.cpp` — building a query buffer, then parsing it back
  into a Message with `Parser::parse()`.
- `resolve_query.cpp` — the full request/response cycle through a
  ZoneStore-backed Resolver.
- `zone_store_basics.cpp` — adding, looking up, and removing records
  in a ZoneStore, including `contains()` vs. `hasName()`.
