// Malformed query handling integration test suite.
//
// Coverage:
// - A query buffer Parser::parse() can't parse produces the same
//   Status through Resolver::resolve() as parsing it directly would,
//   without crashing, and response stays untouched (never built, since
//   resolve() returns before Builder::build() would run)

#include <support/framework.h>

using namespace DnsPro;

namespace {

Vector<std::byte> buildValidQuery() {
    Packet::Name name;
    name.labels.push_back("example");
    name.labels.push_back("com");

    Packet::Question question;
    question.qname = name;
    question.qtype = 1;  // A
    question.qclass = 1; // IN

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

} // namespace

// Verifies a severely truncated query produces the expected Status
// through resolve(), and doesn't crash.
static void truncated_query_status_propagates() {
    Vector<std::byte> valid = buildValidQuery();
    std::span<const std::byte> truncated(valid.begin(), 5); // well short of the 12-byte header

    ZoneStore store;
    Resolver resolver(store);

    Vector<std::byte> response;
    Status s = resolver.resolve(truncated, response);

    CHK(s == Status::BUFFER_TOO_SMALL);
    CHK(response.size() == 0); // never reached Builder::build()
}

// Executes all malformed-query-handling test cases.
static void run_tests() {
    RUN(truncated_query_status_propagates);
}

REGISTER_TEST_SUITE();
