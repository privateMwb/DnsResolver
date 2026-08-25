// Section Count Mismatch.
//
// Mistake: a header claiming more entries in a section than the buffer
// actually contains (e.g. ancount = 3 but only one record follows).
// Note the actual result below -- Parser::parse() doesn't special-case
// this into Status::SECTION_COUNT_MISMATCH; it just keeps trying to read
// the claimed number of records and runs out of buffer partway through,
// surfacing as BUFFER_TOO_SMALL from whichever primitive read failed
// first. SECTION_COUNT_MISMATCH is reserved in Status.h for this
// situation conceptually, but this build reports it via the same
// bounds-check path as any other short read.
//
// Correct pattern alongside it: ancount matching the actual answer count.

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Mistake: ancount Claims 3, Only 1 Record Present");

    Message message;
    Question q;
    q.qname.labels.push_back("example");
    q.qname.labels.push_back("com");
    q.qtype = 1;
    q.qclass = 1;
    message.questions.push_back(q);

    ResourceRecord rr;
    rr.name = q.qname;
    rr.type = 1;
    rr.rclass = 1;
    rr.ttl = 300;
    rr.rdata.push_back(std::byte{192});
    rr.rdata.push_back(std::byte{0});
    rr.rdata.push_back(std::byte{2});
    rr.rdata.push_back(std::byte{1});
    message.answers.push_back(rr); // only one answer

    Vector<std::byte> buffer;
    (void)Builder::build(message, buffer); // header.ancount is correctly written as 1 here

    // Now corrupt the built header to claim 3 answers instead of 1
    // (bytes 6-7 of the header, big-endian ancount).
    buffer[6] = std::byte{0};
    buffer[7] = std::byte{3};

    Message parsed;
    Status status =
        Parser::parse(std::span<const std::byte>(buffer.begin(), buffer.size()), parsed);
    std::cout << "parse() status : " << static_cast<int>(status)
              << "  (BUFFER_TOO_SMALL -- ran out reading the 2nd claimed answer)\n\n";

    setTitle("Correct: ancount Matches What's Actually Present");

    Vector<std::byte> honestBuffer;
    (void)Builder::build(message, honestBuffer); // ancount derived from answers.size() == 1

    Message honestParsed;
    Status honestStatus = Parser::parse(
        std::span<const std::byte>(honestBuffer.begin(), honestBuffer.size()), honestParsed);
    std::cout << "parse() status : " << static_cast<int>(honestStatus) << "  (expect OK)\n";
    std::cout << "answers parsed : " << honestParsed.answers.size() << "\n";
}

REGISTER_EXAMPLE_SUITE();
