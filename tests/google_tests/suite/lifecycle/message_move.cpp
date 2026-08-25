// Message move lifecycle test suite.
//
// Coverage:
// - Move-construct transfers a populated Message's data to the new
//   object, leaving the source in a valid, empty state
// - Move-assign does the same into an already-existing destination

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

Packet::Message makePopulatedMessage() {
    Packet::Message message;
    message.header.id = 0xABCD;
    message.header.qr = 1;

    Packet::Question question;
    question.qname.labels.push_back("example");
    question.qname.labels.push_back("com");
    question.qtype = 1;  // A
    question.qclass = 1; // IN
    message.questions.push_back(question);

    Packet::ResourceRecord record;
    record.name = question.qname;
    record.type = 1;
    record.rclass = 1;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{1});
    message.answers.push_back(record);

    return message;
}

} // namespace

// Verifies move-construct transfers data and leaves the source empty.
TEST(MessageMove, MoveConstructTransfersData) {
    Packet::Message source = makePopulatedMessage();

    Packet::Message destination(std::move(source));

    EXPECT_EQ(destination.header.id, 0xABCD);
    EXPECT_EQ(destination.questions.size(), 1);
    EXPECT_EQ(destination.answers.size(), 1);

    // Vector's move constructor steals the buffer, so the source is left
    // holding empty (but valid) vectors.
    EXPECT_EQ(source.questions.size(), 0);
    EXPECT_EQ(source.answers.size(), 0);
}

// Verifies move-assign transfers data into an existing destination and
// leaves the source empty.
TEST(MessageMove, MoveAssignTransfersData) {
    Packet::Message source = makePopulatedMessage();
    Packet::Message destination = makePopulatedMessage();
    destination.header.id = 0x0000; // distinguishable from source's id before the assign

    destination = std::move(source);

    EXPECT_EQ(destination.header.id, 0xABCD);
    EXPECT_EQ(destination.questions.size(), 1);
    EXPECT_EQ(destination.answers.size(), 1);

    EXPECT_EQ(source.questions.size(), 0);
    EXPECT_EQ(source.answers.size(), 0);
}
