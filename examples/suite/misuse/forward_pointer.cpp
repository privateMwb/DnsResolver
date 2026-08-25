// Forward Pointer.
//
// Mistake: hand-assembling (or receiving from an untrusted source) a
// buffer where a compression pointer targets an offset at or after its
// own position. RFC 1035 S4.1.4 only allows pointers to point strictly
// backward; Parser::parseName() rejects anything else outright rather
// than risk an unbounded or self-referential chain.
//
// Correct pattern alongside it: a normal, uncompressed name parses fine.

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Mistake: a Self-Referencing Pointer");

    // 12-byte header (qdcount = 1), then at offset 12 a pointer that
    // targets offset 12 -- itself.
    Vector<std::byte> bad;
    for (int i = 0; i < 4; ++i)
        bad.push_back(std::byte{0}); // id
    bad.push_back(std::byte{0});     // flags1
    bad.push_back(std::byte{0});     // flags2
    bad.push_back(std::byte{0});
    bad.push_back(std::byte{1}); // qdcount = 1
    bad.push_back(std::byte{0});
    bad.push_back(std::byte{0}); // ancount
    bad.push_back(std::byte{0});
    bad.push_back(std::byte{0}); // nscount
    bad.push_back(std::byte{0});
    bad.push_back(std::byte{0}); // arcount

    bad.push_back(std::byte{0xC0}); // pointer, high byte of offset 12 (0x000C)
    bad.push_back(std::byte{0x0C}); // low byte -- points back at itself
    bad.push_back(std::byte{0});    // qtype high
    bad.push_back(std::byte{1});    // qtype low
    bad.push_back(std::byte{0});    // qclass high
    bad.push_back(std::byte{1});    // qclass low

    Message parsed;
    Status status = Parser::parse(std::span<const std::byte>(bad.begin(), bad.size()), parsed);
    std::cout << "parse() status : " << static_cast<int>(status)
              << "  (expect COMPRESSION_FORWARD_POINTER)\n\n";

    setTitle("Correct: an Uncompressed Name Parses Fine");

    Message goodMessage;
    Question q;
    q.qname.labels.push_back("example");
    q.qname.labels.push_back("com");
    q.qtype = 1;
    q.qclass = 1;
    goodMessage.questions.push_back(q);

    Vector<std::byte> good;
    (void)Builder::build(goodMessage, good);

    Message goodParsed;
    Status goodStatus =
        Parser::parse(std::span<const std::byte>(good.begin(), good.size()), goodParsed);
    std::cout << "parse() status : " << static_cast<int>(goodStatus) << "  (expect OK)\n";
}

REGISTER_EXAMPLE_SUITE();
