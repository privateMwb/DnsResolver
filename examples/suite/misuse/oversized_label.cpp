// Oversized Label.
//
// Mistake: putting more than 63 bytes into a single Name label. RFC 1035
// S3.1 caps each label's encoded length at 63 (it's stored in a 6-bit
// length byte); Builder::writeName() checks this and refuses to write a
// name it can't encode correctly, rather than silently truncating it.
//
// Correct pattern alongside it: a 63-byte label -- right at the limit --
// builds without issue.

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Mistake: a 64-Byte Label");

    Message tooLong;
    Question q;
    q.qname.labels.push_back(std::string(64, 'a')); // one byte over the limit
    q.qname.labels.push_back("com");
    q.qtype = 1;
    q.qclass = 1;
    tooLong.questions.push_back(q);

    Vector<std::byte> badBuffer;
    Status status = Builder::build(tooLong, badBuffer);
    std::cout << "build() status : " << static_cast<int>(status) << "  (expect LABEL_TOO_LONG)\n\n";

    setTitle("Correct: a 63-Byte Label Is Fine");

    Message atLimit;
    Question q2;
    q2.qname.labels.push_back(std::string(63, 'a')); // exactly at the limit
    q2.qname.labels.push_back("com");
    q2.qtype = 1;
    q2.qclass = 1;
    atLimit.questions.push_back(q2);

    Vector<std::byte> goodBuffer;
    Status goodStatus = Builder::build(atLimit, goodBuffer);
    std::cout << "build() status : " << static_cast<int>(goodStatus) << "  (expect OK)\n";
    std::cout << "buffer size    : " << goodBuffer.size() << " bytes\n";
}

REGISTER_EXAMPLE_SUITE();
