// DnsResolver ZoneStore Lookup Benchmark Suite
// Measures ZoneStore::lookup() performance across its three possible
// outcomes - a matching name+type, a name that doesn't exist at all,
// and a name that exists but doesn't hold the requested type - each
// against an otherwise-identical single-record store, isolating the
// lookup cost itself from any Parser/Builder/Resolver pipeline overhead.
//
// A single record is added once outside the timed section per case;
// every iteration then repeats the same lookup, matching the
// SMALL/MEDIUM/LARGE iteration tiers this suite runs at (see the suite
// README).
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - lookup() on a name+type already present in the store
// - lookup() on a name that doesn't exist (NXDOMAIN)
// - lookup() on a name that exists, but not with the requested type (NODATA)

#include <support/framework.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;       // A
constexpr std::uint16_t kOtherType = 28; // AAAA, deliberately not stored
constexpr std::uint16_t kClass = 1;      // IN

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

ZoneStore makeStoreWithOneRecord() {
    ZoneStore store;

    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{34});

    store.addRecord(record);
    return store;
}

} // namespace

// Measures lookup() on a name+type the store already holds.
static void bench_lookup_hit() {
    ZoneStore store = makeStoreWithOneRecord();
    Packet::Name name = makeName("example", "com");

    auto f = [&] {
        Vector<Packet::ResourceRecord> matches = store.lookup(name, kType);
        (void)matches;
    };

    BENCH("Existing name+type", f);
}

// Measures lookup() on a name the store has no entry for at all.
static void bench_lookup_miss() {
    ZoneStore store = makeStoreWithOneRecord();
    Packet::Name name = makeName("missing", "com");

    auto f = [&] {
        Vector<Packet::ResourceRecord> matches = store.lookup(name, kType);
        (void)matches;
    };

    BENCH("Missing name", f);
}

// Measures lookup() on a name the store holds, but not with this type.
static void bench_lookup_nodata() {
    ZoneStore store = makeStoreWithOneRecord();
    Packet::Name name = makeName("example", "com");

    auto f = [&] {
        Vector<Packet::ResourceRecord> matches = store.lookup(name, kOtherType);
        (void)matches;
    };

    BENCH("Existing name, missing type", f);
}

// Executes all lookup benchmark cases.
static void run_benchmarks() {
    bench_lookup_hit();
    std::cout << "\n";

    bench_lookup_miss();
    std::cout << "\n";

    bench_lookup_nodata();
}

REGISTER_BENCH_SUITE();
