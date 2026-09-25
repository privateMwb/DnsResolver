// Resolver zone reference lifecycle test suite.
//
// Coverage:
// - Resolver holds a reference to its ZoneStore, not a copy - a
//   record added to the store after the Resolver was constructed is
//   still visible to that Resolver's resolve() calls

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

Vector<std::byte> buildQuery() {
    Packet::Question question;
    question.qname = makeName("example", "com");
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

// Verifies a record added after Resolver construction is visible to
// that same Resolver's subsequent resolve() calls.
TEST(ResolverZoneReference, ResolverSeesLaterZoneChanges) {
    ZoneStore store; // empty at construction time
    Resolver resolver(store);

    Vector<std::byte> query = buildQuery();
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    // Before: store is empty, expect NXDOMAIN.
    Vector<std::byte> firstResponse;
    Status firstStatus = resolver.resolve(queryBuffer, firstResponse);
    ASSERT_EQ(firstStatus, Status::OK);

    std::span<const std::byte> firstBuffer(firstResponse.begin(), firstResponse.size());
    Packet::Message firstParsed;
    ASSERT_EQ(Parser::parse(firstBuffer, firstParsed), Status::OK);
    EXPECT_EQ(firstParsed.header.rcode, 3); // NXDOMAIN
    EXPECT_EQ(firstParsed.answers.size(), 0);

    // Mutate the store directly, after the Resolver already exists.
    store.addRecord(makeRecord());

    // After: same Resolver, same store reference, now finds the record.
    Vector<std::byte> secondResponse;
    Status secondStatus = resolver.resolve(queryBuffer, secondResponse);
    ASSERT_EQ(secondStatus, Status::OK);

    std::span<const std::byte> secondBuffer(secondResponse.begin(), secondResponse.size());
    Packet::Message secondParsed;
    ASSERT_EQ(Parser::parse(secondBuffer, secondParsed), Status::OK);
    EXPECT_EQ(secondParsed.header.rcode, 0); // NOERROR
    EXPECT_EQ(secondParsed.answers.size(), 1);
}
