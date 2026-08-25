// Unterminated Name.
//
// Mistake: a name encoding that runs out of buffer before reaching the
// terminating zero-length label. Every name must end with a 0x00 byte
// (RFC 1035 S3.1); if the buffer ends mid-label or right after the last
// label with no terminator, Parser has nowhere left to read from and
// reports it rather than reading past the end of the buffer.
//
// Correct pattern alongside it: the same labels, properly terminated.

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Mistake: Buffer Ends Mid-Name, No Terminator");

    // 12-byte header (qdcount = 1), then a single label "com" with no
    // terminating zero-length label after it.
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

    bad.push_back(std::byte{3}); // label length 3
    bad.push_back(std::byte{'c'});
    bad.push_back(std::byte{'o'});
    bad.push_back(std::byte{'m'});
    // -- buffer ends here; no terminating 0x00, no qtype/qclass either.

    Message parsed;
    Status status = Parser::parse(std::span<const std::byte>(bad.begin(), bad.size()), parsed);
    std::cout << "parse() status : " << static_cast<int>(status)
              << "  (expect BUFFER_TOO_SMALL)\n\n";

    setTitle("Correct: the Same Label, Properly Terminated");

    Message goodMessage;
    Question q;
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
