// Build Response.
//
// Demonstrates:
// - constructing an authoritative response Message by hand (header flags,
//   one question, one answer record)
// - filling a ResourceRecord's rdata for a simple A record
// - serializing the Message to wire-format bytes with Builder::build()

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Assemble the Response");

    Message response;
    response.header.id = 0x1234;
    response.header.qr = 1;    // response
    response.header.aa = 1;    // authoritative answer
    response.header.rcode = 0; // NOERROR

    Question q;
    q.qname.labels.push_back("example");
    q.qname.labels.push_back("com");
    q.qtype = 1;  // A
    q.qclass = 1; // IN
    response.questions.push_back(q);

    ResourceRecord answer;
    answer.name = q.qname;
    answer.type = 1;   // A
    answer.rclass = 1; // IN
    answer.ttl = 300;
    answer.rdata.push_back(std::byte{192});
    answer.rdata.push_back(std::byte{0});
    answer.rdata.push_back(std::byte{2});
    answer.rdata.push_back(std::byte{1});
    response.answers.push_back(answer);

    std::cout << "questions : " << response.questions.size() << "\n";
    std::cout << "answers   : " << response.answers.size() << "\n\n";

    setTitle("Serialize It");

    Vector<std::byte> buffer;
    Status status = Builder::build(response, buffer);
    std::cout << "build() status : " << static_cast<int>(status) << "\n";
    std::cout << "buffer size    : " << buffer.size() << " bytes\n";

    // Header's ancount is derived from response.answers.size() by
    // Builder::build() -- not from response.header.ancount, which was
    // never set above.
}

REGISTER_EXAMPLE_SUITE();
