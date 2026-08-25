// Parse Message.
//
// Demonstrates:
// - building a small, well-formed query buffer with Builder::build()
// - parsing a raw wire-format buffer into a Message with Parser::parse()
// - checking the returned Status before touching the parsed message
// - reading the Header, Question, and qname back out of the result

#include <support/framework.h>

#include <span>

using namespace DnsPro;

static void run_examples() {
    setTitle("Build a Sample Query");

    // In real usage this buffer would arrive over the wire (UDP/TCP) --
    // it's built locally here just so the example is self-contained.
    Message queryMessage;
    queryMessage.header.id = 0x1234;
    queryMessage.header.rd = 1; // recursion desired

    Question q;
    q.qname.labels.push_back("example");
    q.qname.labels.push_back("com");
    q.qtype = 1;  // A
    q.qclass = 1; // IN
    queryMessage.questions.push_back(q);

    Vector<std::byte> buffer;
    Status buildStatus = Builder::build(queryMessage, buffer);
    std::cout << "build() status : " << static_cast<int>(buildStatus) << "\n";
    std::cout << "buffer size    : " << buffer.size() << " bytes\n\n";

    setTitle("Parse It Back");

    Message parsed;
    Status parseStatus =
        Parser::parse(std::span<const std::byte>(buffer.begin(), buffer.size()), parsed);
    std::cout << "parse() status : " << static_cast<int>(parseStatus) << "\n\n";

    if (parseStatus != Status::OK) {
        return;
    }

    setTitle("Inspect the Parsed Message");

    std::cout << "header id      : " << parsed.header.id << "\n";
    std::cout << "qdcount        : " << parsed.header.qdcount << "\n";
    std::cout << "question count : " << parsed.questions.size() << "\n";

    const Name& qname = parsed.questions[0].qname;
    std::cout << "qname labels   : ";
    for (std::size_t i = 0; i < qname.labels.size(); ++i) {
        if (i > 0)
            std::cout << ".";
        std::cout << qname.labels[i];
    }
    std::cout << "\n";
    std::cout << "qtype          : " << parsed.questions[0].qtype << "\n";
}

REGISTER_EXAMPLE_SUITE();
