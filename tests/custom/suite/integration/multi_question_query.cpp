// Multi-question query integration test suite.
//
// Coverage:
// - A query with more than one question doesn't crash, and the
//   response echoes back and answers only the first question - per
//   Resolver::resolve()'s documented behavior

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

Packet::Question makeQuestion(const std::string& first, const std::string& second) {
    Packet::Question question;
    question.qname = makeName(first, second);
    question.qtype = kType;
    question.qclass = kClass;
    return question;
}

} // namespace

// Verifies a two-question query doesn't crash and only answers the first.
static void multi_question_answers_first_only() {
    ZoneStore store;

    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{34});

    store.addRecord(record);

    Resolver resolver(store);

    Packet::Message message;
    message.questions.push_back(makeQuestion("example", "com")); // has a record
    message.questions.push_back(makeQuestion("other", "com"));   // no record, irrelevant either way

    Vector<std::byte> query;
    Status buildStatus = Builder::build(message, query);
    CHK(buildStatus == Status::OK);

    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    CHK(resolveStatus == Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());

    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    CHK(parseStatus == Status::OK);

    CHK(parsed.questions.size() == 1);
    CHK(parsed.questions[0].qname.labels[0] == "example");
    CHK(parsed.answers.size() == 1);
    CHK(parsed.header.rcode == 0);
}

// Executes all multi-question-query test cases.
static void run_tests() {
    RUN(multi_question_answers_first_only);
}

REGISTER_TEST_SUITE();