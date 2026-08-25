// Empty zone query integration test suite.
//
// Coverage:
// - Resolving any query against a ZoneStore with zero records returns
//   a well-formed NXDOMAIN response, not a crash or a malformed one

#include <support/framework.h>

using namespace DnsPro;

namespace {

Vector<std::byte> buildQuery() {
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

// Verifies a query against an empty ZoneStore returns a clean NXDOMAIN.
static void empty_zone_returns_nxdomain() {
    ZoneStore store; // deliberately empty, zero addRecord() calls
    Resolver resolver(store);

    Vector<std::byte> query = buildQuery();
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    Vector<std::byte> response;
    Status resolveStatus = resolver.resolve(queryBuffer, response);
    CHK(resolveStatus == Status::OK);

    std::span<const std::byte> responseBuffer(response.begin(), response.size());
    Packet::Message parsed;
    Status parseStatus = Parser::parse(responseBuffer, parsed);
    CHK(parseStatus == Status::OK);

    CHK(parsed.header.qr == 1);
    CHK(parsed.header.rcode == 3); // NXDOMAIN
    CHK(parsed.answers.size() == 0);
}

// Executes all empty-zone-query test cases.
static void run_tests() {
    RUN(empty_zone_returns_nxdomain);
}

REGISTER_TEST_SUITE();
