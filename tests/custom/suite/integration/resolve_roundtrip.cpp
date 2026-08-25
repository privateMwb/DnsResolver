// Resolve round-trip integration test suite.
//
// Coverage:
// - Raw query bytes through Resolver::resolve() produce a raw response
//   that re-parses correctly and carries the right answer
// - The NXDOMAIN case produces a response that re-parses correctly too,
//   with the right rcode and no answers

#include <support/framework.h>

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

Vector<std::byte> buildQuery(const std::string& first, const std::string& second) {
    Packet::Question question;
    question.qname = makeName(first, second);
    question.qtype = kType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status status = Builder::build(message, bytes);
    CHK(status == Status::OK);

    return bytes;
}

} // namespace

// Verifies a full round trip for a query the zone can answer.
static void resolve_roundtrip_hit() {
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

    Resolver resolver(store);

    Vector<std::byte> query = buildQuery("example", "com");
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    CHK(resolveStatus == Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());

    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    CHK(parseStatus == Status::OK);

    CHK(parsed.header.qr == 1);
    CHK(parsed.header.aa == 1);
    CHK(parsed.header.rcode == 0);
    CHK(parsed.answers.size() == 1);
    CHK(parsed.answers[0].type == kType);
    CHK(parsed.answers[0].rdata.size() == 4);
}

// Verifies a full round trip for a query the zone has no answer for.
static void resolve_roundtrip_nxdomain() {
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

    Resolver resolver(store);

    Vector<std::byte> query = buildQuery("missing", "com");
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    CHK(resolveStatus == Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());

    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    CHK(parseStatus == Status::OK);

    CHK(parsed.header.qr == 1);
    CHK(parsed.header.rcode == 3); // NXDOMAIN
    CHK(parsed.answers.size() == 0);
}

// Executes all resolve-roundtrip test cases.
static void run_tests() {
    RUN(resolve_roundtrip_hit);
    RUN(resolve_roundtrip_nxdomain);
}

REGISTER_TEST_SUITE();