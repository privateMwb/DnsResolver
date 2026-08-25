// DnsResolver Canonicalize Benchmark Suite
// Measures ZoneStore's private name canonicalization step in isolation
// -- joining labels with '.' and lowercasing them into the HashMap key
// -- via addRecord(), the cheapest public entry point that exercises it
// without any other ZoneStore work dominating the call (addRecord()'s
// own append cost is O(1); canonicalize() is the only part of it whose
// cost scales with the name itself).
//
// Covers:
// - Name canonicalization via addRecord(), on a mixed-case name

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

Packet::ResourceRecord makeRecord() {
    Packet::ResourceRecord record;
    record.name.labels.push_back("Example");
    record.name.labels.push_back("COM");
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

// Measures canonicalize() indirectly, through addRecord() on a
// mixed-case name.
static void Canonicalize_MixedCaseName(benchmark::State& state) {
    ZoneStore store;
    store.addRecord(makeRecord()); // seeds the bucket so every call below appends

    for (auto _ : state) {
        store.addRecord(makeRecord());
    }
}
BENCHMARK(Canonicalize_MixedCaseName);
