// DnsResolver Zone Size Growth Benchmark Suite
// Measures how ZoneStore::lookup() cost changes as the number of
// distinct names stored grows - hash-table load-factor effects that
// lookup.cpp's fixed single-record store can't show.
//
// Covers:
// - lookup() cost with 100, 1,000, and 10,000 distinct names stored

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

Packet::ResourceRecord makeRecord(const std::string& host) {
    Packet::ResourceRecord record;
    record.name = makeName(host, "example");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{34});
    return record;
}

// Measures lookup() cost with a given number of distinct names stored.
void LookupAt(benchmark::State& state, std::size_t nameCount) {
    ZoneStore store;
    for (std::size_t i = 0; i < nameCount; ++i) {
        store.addRecord(makeRecord("host" + std::to_string(i)));
    }

    Packet::Name target = makeName("host0", "example");

    for (auto _ : state) {
        Vector<Packet::ResourceRecord> matches = store.lookup(target, kType);
        benchmark::DoNotOptimize(matches);
    }
}

} // namespace

BENCHMARK_CAPTURE(LookupAt, Names100, 100);
BENCHMARK_CAPTURE(LookupAt, Names1000, 1000);
BENCHMARK_CAPTURE(LookupAt, Names10000, 10000);
