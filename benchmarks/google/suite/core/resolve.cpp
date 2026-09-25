// DnsResolver Resolver Resolve Benchmark Suite
// Measures Resolver::resolve() end to end - parse, ZoneStore lookup,
// build - across its three possible outcomes, mirroring lookup.cpp's
// hit/miss/nodata split but through the full pipeline instead of
// ZoneStore::lookup() alone.
//
// Each query buffer is built once (via Builder::build()) outside the
// timed loop; every iteration then re-resolves the same buffer against
// the same store. Google Benchmark's own --benchmark_min_time /
// iteration count handles the repeat-until-stable behavior the custom
// suite's SMALL/MEDIUM/LARGE tiers cover by hand.
//
// Covers:
// - resolve() on a query the store can answer
// - resolve() on a query for a name that doesn't exist (NXDOMAIN)
// - resolve() on a query for a name that exists, but not with this type (NODATA)

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

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
static void Resolve_AnswerFound(benchmark::State& state) {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);
    Vector<std::byte> query = buildQuery("example", "com", kType);
    std::span<const std::byte> buffer(query.begin(), query.size());

    for (auto _ : state) {
        Vector<std::byte> response;
        Status s = resolver.resolve(buffer, response);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(response);
    }
}
BENCHMARK(Resolve_AnswerFound);

// Measures resolve() on a query for a name that doesn't exist.
static void Resolve_NXDOMAIN(benchmark::State& state) {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);
    Vector<std::byte> query = buildQuery("missing", "com", kType);
    std::span<const std::byte> buffer(query.begin(), query.size());

    for (auto _ : state) {
        Vector<std::byte> response;
        Status s = resolver.resolve(buffer, response);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(response);
    }
}
BENCHMARK(Resolve_NXDOMAIN);

// Measures resolve() on a query for a name that exists, but not with this type.
static void Resolve_NODATA(benchmark::State& state) {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);
    Vector<std::byte> query = buildQuery("example", "com", kOtherType);
    std::span<const std::byte> buffer(query.begin(), query.size());

    for (auto _ : state) {
        Vector<std::byte> response;
        Status s = resolver.resolve(buffer, response);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(response);
    }
}
BENCHMARK(Resolve_NODATA);
