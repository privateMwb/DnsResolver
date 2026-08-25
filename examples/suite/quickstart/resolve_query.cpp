// Resolve Query.
//
// Demonstrates:
// - populating a ZoneStore and handing it to a Resolver
// - building a raw query buffer and calling Resolver::resolve()
// - parsing the raw response back to inspect its rcode and answers

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Set Up the Zone");

    ZoneStore zone;

    ResourceRecord a;
    a.name.labels.push_back("example");
    a.name.labels.push_back("com");
    a.type = 1;   // A
    a.rclass = 1; // IN
    a.ttl = 300;
    a.rdata.push_back(std::byte{192});
    a.rdata.push_back(std::byte{0});
    a.rdata.push_back(std::byte{2});
    a.rdata.push_back(std::byte{1});
    zone.addRecord(a);

    Resolver resolver(zone);

    setTitle("Build and Send a Query");

    Message queryMessage;
    Question q;
    q.qname = a.name;
    q.qtype = 1;  // A
    q.qclass = 1; // IN
    queryMessage.questions.push_back(q);

    Vector<std::byte> query;
    Status buildStatus = Builder::build(queryMessage, query);
    std::cout << "build() status : " << static_cast<int>(buildStatus) << "\n";

    Vector<std::byte> response;
    Status resolveStatus =
        resolver.resolve(std::span<const std::byte>(query.begin(), query.size()), response);
    std::cout << "resolve() status : " << static_cast<int>(resolveStatus) << "\n\n";

    setTitle("Inspect the Response");

    Message parsedResponse;
    Status parseStatus = Parser::parse(
        std::span<const std::byte>(response.begin(), response.size()), parsedResponse);
    std::cout << "parse() status : " << static_cast<int>(parseStatus) << "\n";
    if (parseStatus != Status::OK)
        return;

    std::cout << "qr             : " << static_cast<int>(parsedResponse.header.qr) << "\n";
    std::cout << "aa             : " << static_cast<int>(parsedResponse.header.aa) << "\n";
    std::cout << "rcode          : " << static_cast<int>(parsedResponse.header.rcode) << "\n";
    std::cout << "answer count   : " << parsedResponse.answers.size() << "\n";
}

REGISTER_EXAMPLE_SUITE();
