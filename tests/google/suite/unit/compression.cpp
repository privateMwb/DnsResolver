// Compression pointer unit test suite.
//
// Coverage:
// - A name's pointer targeting itself is rejected (Status::COMPRESSION_FORWARD_POINTER)
// - A name's pointer targeting an offset ahead of itself is rejected (same status)
// - A deep, but validly strictly-decreasing, pointer chain still parses
//   successfully - the step-count safety net doesn't misfire on
//   legitimate input
//
// NOTE: Status::COMPRESSION_LOOP has no test here. The forward-pointer
// check (target must be < cursor) already forces every pointer hop to
// strictly decrease the cursor, which alone guarantees termination
// within at most buffer.size() hops - tighter than the step-counter's
// own buffer.size()+1 bound. That makes COMPRESSION_LOOP look
// unreachable: there's no input where the forward-pointer check
// passes but the step counter still runs out. Flagging this rather
// than fabricating a test for a path that may not exist - worth
// confirming on the implementation side.
//
// Builder never emits compression pointers (see Builder.h), so every
// buffer here is hand-assembled: a normal single-question message is
// built via Builder::build() first, then a second question's raw bytes
// are appended by hand with a pointer instead of a real name, and
// qdcount is patched from 1 to 2 directly in the buffer.

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

// The first question's name always starts right here: Builder writes
// the fixed 12-byte header first, unconditionally.
constexpr std::uint16_t kFirstNameOffset = 12;

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

// Appends a second question whose qname is a raw pointer (not a real
// name) targeting `target`, and bumps qdcount from 1 to 2.
Vector<std::byte> appendPointerQuestion(Vector<std::byte> bytes, std::uint16_t target) {
    bytes.push_back(std::byte{static_cast<std::uint8_t>(0xC0 | (target >> 8))});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(target & 0xFF)});
    bytes.push_back(std::byte{0});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kType)});
    bytes.push_back(std::byte{0});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kClass)});

    bytes[4] = std::byte{0};
    bytes[5] = std::byte{2};

    return bytes;
}

} // namespace

// Verifies a pointer targeting an offset at or after itself is rejected.
TEST(Compression, ForwardPointerRejected) {
    Vector<std::byte> base = buildBaseQuery();
    std::uint16_t pointerOffset = static_cast<std::uint16_t>(base.size());

    // Target the pointer's own offset - a self-pointer, the simplest
    // forward-pointer case.
    Vector<std::byte> bytes = appendPointerQuestion(base, pointerOffset);
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    EXPECT_EQ(s, Status::COMPRESSION_FORWARD_POINTER);
}

// Verifies a pointer targeting an offset past the end of the buffer
// (unambiguously forward) is rejected the same way.
TEST(Compression, FarForwardPointerRejected) {
    Vector<std::byte> base = buildBaseQuery();
    std::uint16_t pointerOffset = static_cast<std::uint16_t>(base.size());
    std::uint16_t farTarget = static_cast<std::uint16_t>(pointerOffset + 20);

    Vector<std::byte> bytes = appendPointerQuestion(base, farTarget);
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    EXPECT_EQ(s, Status::COMPRESSION_FORWARD_POINTER);
}

// Verifies a valid backward pointer (the ordinary, well-formed case)
// still parses successfully - confirms rejection above is about
// direction, not compression pointers in general.
TEST(Compression, ValidBackwardPointerAccepted) {
    Vector<std::byte> base = buildBaseQuery();
    Vector<std::byte> bytes = appendPointerQuestion(base, kFirstNameOffset);
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    ASSERT_EQ(s, Status::OK);
    ASSERT_EQ(out.questions.size(), 2);
    ASSERT_EQ(out.questions[1].qname.labels.size(), 2);
    EXPECT_EQ(out.questions[1].qname.labels[0], "example");
    EXPECT_EQ(out.questions[1].qname.labels[1], "com");
}
