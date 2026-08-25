// DnsResolver ZoneStore Record Benchmark Suite
// Measures ZoneStore::addRecord() and ZoneStore::removeRecord()
// performance, isolated from Parser/Builder/Resolver pipeline overhead.
//
// addRecord() is benchmarked against a store that already holds one
// record under the target name+type, so every call appends to an
// existing bucket (steady-state Vector::push_back cost) rather than
// paying HashMap-insert cost on every call. The bucket does grow over
// the course of a run -- that's an accurate reflection of what
// repeated addRecord() calls under one name actually do (round-robin
// records never get deduplicated), not a benchmarking artifact.
//
// removeRecord() only has a repeatable benchmark on its not-found path:
// a successful removal empties (or shrinks) the store, so a "hit" case
// can't be called across many iterations without either re-adding the
// record between calls (which would fold addRecord() cost into the
// measurement) or exhausting a large pre-seeded record set. The
// not-found path is exercised here instead -- name lookup fails before
// any removal work would happen -- distinct from lookup.cpp since it's
// still ZoneStore::removeRecord() specifically, not lookup().
//
// A single record is added once outside the timed loop per case; every
// iteration then repeats the same call. Google Benchmark's own
// --benchmark_min_time / iteration count handles the repeat-until-stable
// behavior the custom suite's SMALL/MEDIUM/LARGE tiers cover by hand.
//
// Covers:
// - addRecord() appending to an existing name+type bucket
// - removeRecord() on a name the store has no entry for

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::ResourceRecord makeRecord() {
    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{34});
    return record;
}

} // namespace

// Measures addRecord() appending to a name+type bucket that already exists.
static void AddRecord_ExistingBucket(benchmark::State& state) {
    ZoneStore store;
    store.addRecord(makeRecord());

    for (auto _ : state) {
        store.addRecord(makeRecord());
    }
}
BENCHMARK(AddRecord_ExistingBucket);

// Measures removeRecord() on a name the store has no entry for at all.
static void RemoveRecord_MissingName(benchmark::State& state) {
    ZoneStore store;
    store.addRecord(makeRecord());

    Packet::Name missingName = makeName("missing", "com");

    for (auto _ : state) {
        bool removed = store.removeRecord(missingName, kType);
        benchmark::DoNotOptimize(removed);
    }
}
BENCHMARK(RemoveRecord_MissingName);
