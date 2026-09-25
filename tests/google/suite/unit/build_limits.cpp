// Builder limit-check unit test suite.
//
// Coverage:
// - A label over 63 bytes is rejected (Status::LABEL_TOO_LONG)
// - A section with more than 65535 entries is rejected
//   (Status::SECTION_TOO_LARGE), since qdcount/ancount/nscount/arcount
//   are 16-bit fields
// - rdata too large for the 16-bit rdlength field is rejected
//   (Status::RDATA_LENGTH_MISMATCH), same status Parser uses for the
//   reverse (too-large-to-fit) case on the read side

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

} // namespace

// Verifies a label over 63 bytes is rejected.
TEST(BuildLimits, OversizedLabelRejected) {
    std::string oversizedLabel(64, 'a'); // one byte past the 63-byte limit

    Packet::Question question;
    question.qname = makeName(oversizedLabel, "com");
    question.qtype = kType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    EXPECT_EQ(s, Status::LABEL_TOO_LONG);
}

// Verifies a label at exactly 63 bytes (the boundary itself) is accepted.
TEST(BuildLimits, BoundaryLabelAccepted) {
    std::string boundaryLabel(63, 'a'); // exactly at the limit

    Packet::Question question;
    question.qname = makeName(boundaryLabel, "com");
    question.qtype = kType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    EXPECT_EQ(s, Status::OK);
}

// Verifies a section with more than 65535 entries is rejected.
TEST(BuildLimits, OversizedSectionRejected) {
    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{34});

    Packet::Message message;
    for (std::size_t i = 0; i < 0x10000; ++i) { // 65536, one past the 16-bit limit
        message.answers.push_back(record);
    }

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    EXPECT_EQ(s, Status::SECTION_TOO_LARGE);
}

// Verifies rdata too large for the 16-bit rdlength field is rejected.
TEST(BuildLimits, OversizedRdataRejected) {
    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    for (std::size_t i = 0; i < 0x10000; ++i) { // 65536 bytes, one past the 16-bit limit
        record.rdata.push_back(std::byte{0});
    }

    Packet::Message message;
    message.answers.push_back(record);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    EXPECT_EQ(s, Status::RDATA_LENGTH_MISMATCH);
}
