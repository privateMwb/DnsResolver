// Resolve round-trip integration test suite.
//
// Coverage:
// - Raw query bytes through Resolver::resolve() produce a raw response
//   that re-parses correctly and carries the right answer
// - The NXDOMAIN case produces a response that re-parses correctly too,
//   with the right rcode and no answers

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

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

Vector<std::byte> buildQuery(const std::string& first, const std::string& second) {
    Packet::Question question;
    question.qname = makeName(first, second);
    question.qtype = kType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

} // namespace

// Verifies a full round trip for a query the zone can answer.
TEST(ResolveRoundtrip, ResolveRoundtripHit) {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);

    Vector<std::byte> query = buildQuery("example", "com");
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    ASSERT_EQ(resolveStatus, Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());
    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    ASSERT_EQ(parseStatus, Status::OK);

    EXPECT_EQ(parsed.header.qr, 1);
    EXPECT_EQ(parsed.header.aa, 1);
    EXPECT_EQ(parsed.header.rcode, 0);
    ASSERT_EQ(parsed.answers.size(), 1);
    EXPECT_EQ(parsed.answers[0].type, kType);
    EXPECT_EQ(parsed.answers[0].rdata.size(), 4);
}

// Verifies a full round trip for a query the zone has no answer for.
TEST(ResolveRoundtrip, ResolveRoundtripNxdomain) {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);

    Vector<std::byte> query = buildQuery("missing", "com");
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    ASSERT_EQ(resolveStatus, Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());
    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    ASSERT_EQ(parseStatus, Status::OK);

    EXPECT_EQ(parsed.header.qr, 1);
    EXPECT_EQ(parsed.header.rcode, 3); // NXDOMAIN
    EXPECT_EQ(parsed.answers.size(), 0);
}
