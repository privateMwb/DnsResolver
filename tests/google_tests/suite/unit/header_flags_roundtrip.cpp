// Header flags round-trip unit test suite.
//
// Coverage:
// - QR/OPCODE/AA/TC/RD/RA/Z/RCODE and id survive Builder::build() then
//   Parser::parse() unchanged, at both extremes (all-zero and
//   all-flags-set)
//
// qdcount/ancount/nscount/arcount are deliberately not checked here -
// Builder::build() derives those from the section vectors' actual
// sizes rather than trusting header.*count on input (see Builder.h),
// so they're not meaningful "round trip" fields the way the flag bits are.

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

Packet::Message wrapHeader(const Packet::Header& header) {
    Packet::Question question;
    question.qname.labels.push_back("example");
    question.qname.labels.push_back("com");
    question.qtype = 1;  // A
    question.qclass = 1; // IN

    Packet::Message message;
    message.header = header;
    message.questions.push_back(question);
    return message;
}

void checkFlagsMatch(const Packet::Header& expected, const Packet::Header& actual) {
    EXPECT_EQ(actual.id, expected.id);
    EXPECT_EQ(actual.qr, expected.qr);
    EXPECT_EQ(actual.opcode, expected.opcode);
    EXPECT_EQ(actual.aa, expected.aa);
    EXPECT_EQ(actual.tc, expected.tc);
    EXPECT_EQ(actual.rd, expected.rd);
    EXPECT_EQ(actual.ra, expected.ra);
    EXPECT_EQ(actual.z, expected.z);
    EXPECT_EQ(actual.rcode, expected.rcode);
}

} // namespace

// Verifies an all-zero header round-trips unchanged.
TEST(HeaderFlagsRoundtrip, AllZeroFlagsRoundtrip) {
    Packet::Header header{};
    header.id = 0;

    Vector<std::byte> bytes;
    Status buildStatus = Builder::build(wrapHeader(header), bytes);
    ASSERT_EQ(buildStatus, Status::OK);

    std::span<const std::byte> buffer(bytes.begin(), bytes.size());
    Packet::Message out;
    Status parseStatus = Parser::parse(buffer, out);
    ASSERT_EQ(parseStatus, Status::OK);

    checkFlagsMatch(header, out.header);
}

// Verifies every flag bit set to its maximum round-trips unchanged.
TEST(HeaderFlagsRoundtrip, AllFlagsSetRoundtrip) {
    Packet::Header header{};
    header.id = 0xBEEF;
    header.qr = 1;
    header.opcode = 0xF; // max 4-bit value
    header.aa = 1;
    header.tc = 1;
    header.rd = 1;
    header.ra = 1;
    header.z = 0x7;     // max 3-bit value
    header.rcode = 0xF; // max 4-bit value

    Vector<std::byte> bytes;
    Status buildStatus = Builder::build(wrapHeader(header), bytes);
    ASSERT_EQ(buildStatus, Status::OK);

    std::span<const std::byte> buffer(bytes.begin(), bytes.size());
    Packet::Message out;
    Status parseStatus = Parser::parse(buffer, out);
    ASSERT_EQ(parseStatus, Status::OK);

    checkFlagsMatch(header, out.header);
}
