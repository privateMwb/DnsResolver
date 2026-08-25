// Compression pointer chain integration test suite.
//
// Coverage:
// - A name reached through a two-hop pointer chain (pointer -> pointer
//   -> real labels) resolves to the correct labels, not just a
//   single-hop pointer
//
// Builder never emits compression pointers (see Builder.h), so this
// buffer is hand-assembled: a base question is built via
// Builder::build(), then two more questions are appended by hand -
// the second a pointer to the first's real name, the third a pointer
// to the second's pointer bytes (not directly to the first's name) -
// forcing Parser to follow a chain, not just resolve one hop.

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;             // A
constexpr std::uint16_t kClass = 1;            // IN
constexpr std::uint16_t kFirstNameOffset = 12; // right after the fixed 12-byte header

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Vector<std::byte> buildBaseQuery() {
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

void appendPointerQuestion(Vector<std::byte>& bytes, std::uint16_t target) {
    bytes.push_back(std::byte{static_cast<std::uint8_t>(0xC0 | (target >> 8))});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(target & 0xFF)});
    bytes.push_back(std::byte{0});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kType)});
    bytes.push_back(std::byte{0});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kClass)});
}

} // namespace

// Verifies a two-hop pointer chain (pointer -> pointer -> labels)
// resolves to the correct name.
TEST(CompressionPointerChain, TwoHopPointerChainResolves) {
    Vector<std::byte> bytes = buildBaseQuery();

    std::uint16_t secondQuestionOffset = static_cast<std::uint16_t>(bytes.size());
    appendPointerQuestion(bytes, kFirstNameOffset); // q2's pointer targets q1's real name

    std::uint16_t thirdQuestionOffset = static_cast<std::uint16_t>(bytes.size());
    (void)thirdQuestionOffset;
    appendPointerQuestion(bytes, secondQuestionOffset); // q3's pointer targets q2's pointer

    // Patch qdcount from 1 to 3.
    bytes[4] = std::byte{0};
    bytes[5] = std::byte{3};

    std::span<const std::byte> buffer(bytes.begin(), bytes.size());
    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    ASSERT_EQ(s, Status::OK);

    ASSERT_EQ(out.questions.size(), 3);
    for (const auto& question : out.questions) {
        ASSERT_EQ(question.qname.labels.size(), 2);
        EXPECT_EQ(question.qname.labels[0], "example");
        EXPECT_EQ(question.qname.labels[1], "com");
    }
}
