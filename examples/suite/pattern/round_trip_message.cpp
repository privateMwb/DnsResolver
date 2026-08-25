// Round Trip Message.
//
// Demonstrates:
// - the build-then-parse round-trip pattern used to validate a Message
// - Builder never emits compression, so a round trip re-parses to an
//   equivalent Message, not necessarily byte-identical wire output
// - comparing the rebuilt Message field-by-field against the original

#include <support/framework.h>

using namespace DnsPro;

static bool namesEqual(const Name& a, const Name& b) {
    if (a.labels.size() != b.labels.size())
        return false;
    for (std::size_t i = 0; i < a.labels.size(); ++i) {
        if (a.labels[i] != b.labels[i])
            return false;
    }
    return true;
}

static void run_examples() {
    setTitle("Construct the Original Message");

    Message original;
    original.header.id = 0xABCD;
    original.header.rd = 1;

    Question q;
    q.qname.labels.push_back("mail");
    q.qname.labels.push_back("example");
    q.qname.labels.push_back("com");
    q.qtype = 15; // MX
    q.qclass = 1; // IN
    original.questions.push_back(q);

    setTitle("Build, Then Parse Back");

    Vector<std::byte> buffer;
    Status buildStatus = Builder::build(original, buffer);
    std::cout << "build() status : " << static_cast<int>(buildStatus) << "\n";

    Message roundTripped;
    Status parseStatus =
        Parser::parse(std::span<const std::byte>(buffer.begin(), buffer.size()), roundTripped);
    std::cout << "parse() status : " << static_cast<int>(parseStatus) << "\n\n";

    setTitle("Compare Against the Original");

    bool idMatches = original.header.id == roundTripped.header.id;
    bool countMatches = original.questions.size() == roundTripped.questions.size();
    bool nameMatches =
        countMatches && namesEqual(original.questions[0].qname, roundTripped.questions[0].qname);
    bool typeMatches =
        countMatches && original.questions[0].qtype == roundTripped.questions[0].qtype;

    std::cout << "header id matches  : " << idMatches << "\n";
    std::cout << "question count matches : " << countMatches << "\n";
    std::cout << "qname matches      : " << nameMatches << "\n";
    std::cout << "qtype matches      : " << typeMatches << "\n";
}

REGISTER_EXAMPLE_SUITE();
