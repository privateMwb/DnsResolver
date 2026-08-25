// Parse/build round-trip integration test suite.
//
// Coverage:
// - A Message with a question, answer, authority, and additional
//   record all populated survives Builder::build() then Parser::parse()
//   with every field intact

#include <support/framework.h>

using namespace DnsPro;

namespace {

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::ResourceRecord makeRecord(const std::string& first, const std::string& second,
                                  std::uint16_t type, std::uint32_t ttl, std::uint8_t lastOctet) {
    Packet::ResourceRecord record;
    record.name = makeName(first, second);
    record.type = type;
    record.rclass = 1; // IN
    record.ttl = ttl;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{lastOctet});
    return record;
}

} // namespace

// Verifies a fully-populated Message survives build()+parse() intact.
static void full_message_roundtrip() {
    Packet::Message original;
    original.header.id = 0x1234;
    original.header.qr = 1;
    original.header.aa = 1;
    original.header.rcode = 0;

    Packet::Question question;
    question.qname = makeName("example", "com");
    question.qtype = 1;  // A
    question.qclass = 1; // IN
    original.questions.push_back(question);

    original.answers.push_back(makeRecord("example", "com", 1, 3600, 10));
    original.authorities.push_back(makeRecord("com", "com", 2, 7200, 20)); // NS-ish
    original.additionals.push_back(makeRecord("ns1", "com", 1, 1800, 30));

    Vector<std::byte> bytes;
    Status buildStatus = Builder::build(original, bytes);
    CHK(buildStatus == Status::OK);

    std::span<const std::byte> buffer(bytes.begin(), bytes.size());
    Packet::Message parsed;
    Status parseStatus = Parser::parse(buffer, parsed);
    CHK(parseStatus == Status::OK);

    CHK(parsed.header.id == original.header.id);
    CHK(parsed.header.qr == original.header.qr);
    CHK(parsed.header.aa == original.header.aa);
    CHK(parsed.header.rcode == original.header.rcode);

    CHK(parsed.questions.size() == 1);
    CHK(parsed.questions[0].qname.labels.size() == 2);
    CHK(parsed.questions[0].qname.labels[0] == "example");
    CHK(parsed.questions[0].qname.labels[1] == "com");
    CHK(parsed.questions[0].qtype == question.qtype);
    CHK(parsed.questions[0].qclass == question.qclass);

    CHK(parsed.answers.size() == 1);
    CHK(parsed.answers[0].type == 1);
    CHK(parsed.answers[0].ttl == 3600);
    CHK(parsed.answers[0].rdata.size() == 4);

    CHK(parsed.authorities.size() == 1);
    CHK(parsed.authorities[0].type == 2);
    CHK(parsed.authorities[0].ttl == 7200);

    CHK(parsed.additionals.size() == 1);
    CHK(parsed.additionals[0].ttl == 1800);
}

// Executes all parse/build round-trip test cases.
static void run_tests() {
    RUN(full_message_roundtrip);
}

REGISTER_TEST_SUITE();
