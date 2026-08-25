// Multi-question query integration test suite.
//
// Coverage:
// - A query with more than one question doesn't crash, and the
//   response echoes back and answers only the first question - per
//   Resolver::resolve()'s documented behavior

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

ZoneStore makeStoreWithOneRecord() {
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
    return store;
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
TEST(MultiQuestionQuery, MultiQuestionAnswersFirstOnly) {
    ZoneStore store = makeStoreWithOneRecord();
    Resolver resolver(store);

    Packet::Message message;
    message.questions.push_back(makeQuestion("example", "com")); // has a record
    message.questions.push_back(makeQuestion("other", "com"));   // no record, irrelevant either way

    Vector<std::byte> query;
    Status buildStatus = Builder::build(message, query);
    ASSERT_EQ(buildStatus, Status::OK);

    std::span<const std::byte> queryBuffer(query.begin(), query.size());
    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    ASSERT_EQ(resolveStatus, Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());
    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    ASSERT_EQ(parseStatus, Status::OK);

    ASSERT_EQ(parsed.questions.size(), 1); // only the first question echoed back
    EXPECT_EQ(parsed.questions[0].qname.labels[0], "example");
    EXPECT_EQ(parsed.answers.size(), 1);
    EXPECT_EQ(parsed.header.rcode, 0);
}
