// DnsResolver Zone Size Growth Benchmark Suite
// Measures how ZoneStore::lookup() cost changes as the number of
// distinct names stored grows - hash-table load-factor effects that
// lookup.cpp's fixed single-record store can't show.
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - lookup() cost with 100, 1,000, and 10,000 distinct names stored

#include <support/framework.h>

#include <iostream>

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

void benchLookupAt(std::size_t nameCount, const char* label) {
    ZoneStore store;
    for (std::size_t i = 0; i < nameCount; ++i) {
        store.addRecord(makeRecord("host" + std::to_string(i)));
    }

    Packet::Name target = makeName("host0", "example");

    auto f = [&] {
        Vector<Packet::ResourceRecord> matches = store.lookup(target, kType);
        (void)matches;
    };

    BENCH(label, f);
}

} // namespace

// Executes all zone-size-growth benchmark cases.
static void run_benchmarks() {
    benchLookupAt(100, "100 names stored");
    std::cout << "\n";

    benchLookupAt(1000, "1,000 names stored");
    std::cout << "\n";

    benchLookupAt(10000, "10,000 names stored");
}

REGISTER_BENCH_SUITE();
