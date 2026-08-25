// Buffer-too-small unit test suite.
//
// Coverage:
// - A buffer shorter than the fixed 12-byte header is rejected
// - A header claiming a question exists, with no question bytes
//   present at all, is rejected
// - A question's name is present but qtype/qclass are cut off, is rejected
// - A resource record's rdlength claims more bytes than the buffer
//   actually has left (Status::RDATA_LENGTH_MISMATCH, not
//   BUFFER_TOO_SMALL - a distinct, more specific status for this
//   particular field, not a stand-in for it)

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

Vector<std::byte> buildAnswerMessage() {
    Packet::Question question;
    question.qname = makeName("example", "com");
    question.qtype = kType;
    question.qclass = kClass;

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
    message.questions.push_back(question);
    message.answers.push_back(record);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

} // namespace

// Verifies a buffer shorter than the fixed 12-byte header is rejected.
static void truncated_header_rejected() {
    Vector<std::byte> bytes = buildQuery();
    std::span<const std::byte> buffer(bytes.begin(), 5); // well short of 12

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    CHK(s == Status::BUFFER_TOO_SMALL);
}

// Verifies a header claiming qdcount=1 with no question bytes at all
// is rejected.
static void truncated_question_name_rejected() {
    Vector<std::byte> bytes = buildQuery();
    std::span<const std::byte> buffer(bytes.begin(), 12); // header only, question cut off entirely

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    CHK(s == Status::BUFFER_TOO_SMALL);
}

// Verifies a question whose name is present, but whose qtype/qclass
// are cut off, is rejected.
static void truncated_question_fields_rejected() {
    Vector<std::byte> bytes = buildQuery();
    // 12 (header) + name ("example" = 8 bytes incl. length byte, "com" =
    // 4 bytes incl. length byte, +1 terminator) = 12 + 13 = 25. Cut 2
    // bytes short of the full 29-byte buffer (25 + qtype(2) + qclass(2)),
    // landing mid-qclass.
    std::size_t truncatedSize = bytes.size() - 2;
    std::span<const std::byte> buffer(bytes.begin(), truncatedSize);

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    CHK(s == Status::BUFFER_TOO_SMALL);
}

// Verifies a resource record whose rdlength claims more bytes than the
// buffer has left is rejected with the rdata-specific status, not the
// generic buffer-too-small one.
static void truncated_rdata_rejected() {
    Vector<std::byte> bytes = buildAnswerMessage();
    std::size_t truncatedSize = bytes.size() - 2; // cut into the 4-byte rdata
    std::span<const std::byte> buffer(bytes.begin(), truncatedSize);

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    CHK(s == Status::RDATA_LENGTH_MISMATCH);
}

// Executes all buffer-too-small test cases.
static void run_tests() {
    RUN(truncated_header_rejected);
    RUN(truncated_question_name_rejected);
    RUN(truncated_question_fields_rejected);
    RUN(truncated_rdata_rejected);
}

REGISTER_TEST_SUITE();
