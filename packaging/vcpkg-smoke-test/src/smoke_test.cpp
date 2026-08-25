// Smoke test for the vcpkg-installed package: confirms the public
// header is reachable and the library links, by round-tripping a small
// Message through Builder::build() and Parser::parse().
//
// Same scope as the Conan test_package smoke test — proves the
// package is installable and linkable, not that its full behavior is
// correct.

#include <DnsPro/DnsResolver.h>

#include <iostream>
#include <span>

using namespace DnsPro;

int main() {
    Message query;
    Question q;
    q.qname.labels.push_back("example");
    q.qname.labels.push_back("com");
    q.qtype = 1;  // A
    q.qclass = 1; // IN
    query.questions.push_back(q);

    Vector<std::byte> buffer;
    Status buildStatus = Builder::build(query, buffer);
    if (buildStatus != Status::OK) {
        std::cerr << "Builder::build() failed, status " << static_cast<int>(buildStatus) << "\n";
        return 1;
    }

    Message parsed;
    Status parseStatus =
        Parser::parse(std::span<const std::byte>(buffer.begin(), buffer.size()), parsed);
    if (parseStatus != Status::OK) {
        std::cerr << "Parser::parse() failed, status " << static_cast<int>(parseStatus) << "\n";
        return 1;
    }

    if (parsed.questions.size() != 1 || parsed.questions[0].qname.labels.size() != 2 ||
        parsed.questions[0].qname.labels[0] != "example" ||
        parsed.questions[0].qname.labels[1] != "com") {
        std::cerr << "Round trip produced unexpected content.\n";
        return 1;
    }

    std::cout << "DnsPro linked and round-tripped a message successfully.\n";
    return 0;
}
