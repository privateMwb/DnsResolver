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

#include <support/framework.h>

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
    CHK(actual.id == expected.id);
    CHK(actual.qr == expected.qr);
    CHK(actual.opcode == expected.opcode);
    CHK(actual.aa == expected.aa);
    CHK(actual.tc == expected.tc);
    CHK(actual.rd == expected.rd);
    CHK(actual.ra == expected.ra);
    CHK(actual.z == expected.z);
    CHK(actual.rcode == expected.rcode);
}

} // namespace

// Verifies an all-zero header round-trips unchanged.
static void all_zero_flags_roundtrip() {
    Packet::Header header{};
    header.id = 0;

    Vector<std::byte> bytes;
    Status buildStatus = Builder::build(wrapHeader(header), bytes);
    CHK(buildStatus == Status::OK);

    std::span<const std::byte> buffer(bytes.begin(), bytes.size());
    Packet::Message out;
    Status parseStatus = Parser::parse(buffer, out);
    CHK(parseStatus == Status::OK);

    checkFlagsMatch(header, out.header);
}

// Verifies every flag bit set to its maximum round-trips unchanged.
static void all_flags_set_roundtrip() {
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
    CHK(buildStatus == Status::OK);

    std::span<const std::byte> buffer(bytes.begin(), bytes.size());
    Packet::Message out;
    Status parseStatus = Parser::parse(buffer, out);
    CHK(parseStatus == Status::OK);

    checkFlagsMatch(header, out.header);
}

// Executes all header-flags-roundtrip test cases.
static void run_tests() {
    RUN(all_zero_flags_roundtrip);
    RUN(all_flags_set_roundtrip);
}

REGISTER_TEST_SUITE();
