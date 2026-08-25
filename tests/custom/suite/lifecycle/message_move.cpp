// Message move lifecycle test suite.
//
// Coverage:
// - Move-construct transfers a populated Message's data to the new
//   object, leaving the source in a valid, empty state
// - Move-assign does the same into an already-existing destination

#include <support/framework.h>

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
static void move_construct_transfers_data() {
    Packet::Message source = makePopulatedMessage();

    Packet::Message destination(std::move(source));

    CHK(destination.header.id == 0xABCD);
    CHK(destination.questions.size() == 1);
    CHK(destination.answers.size() == 1);

    // Vector's move constructor steals the buffer, so the source is left
    // holding empty (but valid) vectors.
    CHK(source.questions.size() == 0);
    CHK(source.answers.size() == 0);
}

// Verifies move-assign transfers data into an existing destination and
// leaves the source empty.
static void move_assign_transfers_data() {
    Packet::Message source = makePopulatedMessage();
    Packet::Message destination = makePopulatedMessage();
    destination.header.id = 0x0000; // distinguishable from source's id before the assign

    destination = std::move(source);

    CHK(destination.header.id == 0xABCD);
    CHK(destination.questions.size() == 1);
    CHK(destination.answers.size() == 1);

    CHK(source.questions.size() == 0);
    CHK(source.answers.size() == 0);
}

// Executes all message-move test cases.
static void run_tests() {
    RUN(move_construct_transfers_data);
    RUN(move_assign_transfers_data);
}

REGISTER_TEST_SUITE();
