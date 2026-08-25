// DnsResolver Resolver Resolve Benchmark Suite
// Measures Resolver::resolve() end to end - parse, ZoneStore lookup,
// build - across its three possible outcomes, mirroring lookup.cpp's
// hit/miss/nodata split but through the full pipeline instead of
// ZoneStore::lookup() alone.
//
// Each query buffer is built once (via Builder::build()) outside the
// timed section; every iteration then re-resolves the same buffer
// against the same store, matching the SMALL/MEDIUM/LARGE iteration
// tiers this suite runs at (see the suite README).
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - resolve() on a query the store can answer
// - resolve() on a query for a name that doesn't exist (NXDOMAIN)
// - resolve() on a query for a name that exists, but not with this type (NODATA)

#include <support/framework.h>

#include <iostream>

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

ZoneStore makeStoreWithOneRecord() {
    ZoneStore store;
    store.addRecord(makeRecord());
    return store;
}

Vector<std::byte> buildQuery(const std::string& first, const std::string& second,
                             std::uint16_t qtype) {
    Packet::Question question;
    question.qname = makeName(first, second);
    question.qtype = qtype;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

} // namespace

// Measures resolve() on a query the store can answer.
static void bench_resolve_hit() {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);
    Vector<std::byte> query = buildQuery("example", "com", kType);
    std::span<const std::byte> buffer(query.begin(), query.size());

    auto f = [&] {
        Vector<std::byte> response;
        Status s = resolver.resolve(buffer, response);
        (void)s;
    };

    BENCH("Answer found", f);
}

// Measures resolve() on a query for a name that doesn't exist.
static void bench_resolve_miss() {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);
    Vector<std::byte> query = buildQuery("missing", "com", kType);
    std::span<const std::byte> buffer(query.begin(), query.size());

    auto f = [&] {
        Vector<std::byte> response;
        Status s = resolver.resolve(buffer, response);
        (void)s;
    };

    BENCH("NXDOMAIN", f);
}

// Measures resolve() on a query for a name that exists, but not with this type.
static void bench_resolve_nodata() {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);
    Vector<std::byte> query = buildQuery("example", "com", kOtherType);
    std::span<const std::byte> buffer(query.begin(), query.size());

    auto f = [&] {
        Vector<std::byte> response;
        Status s = resolver.resolve(buffer, response);
        (void)s;
    };

    BENCH("NODATA", f);
}

// Executes all resolve benchmark cases.
static void run_benchmarks() {
    bench_resolve_hit();
    std::cout << "\n";

    bench_resolve_miss();
    std::cout << "\n";

    bench_resolve_nodata();
}

REGISTER_BENCH_SUITE();
