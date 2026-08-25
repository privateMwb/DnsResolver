// Compression Pointers.
//
// Demonstrates:
// - Parser::parseName() following an RFC 1035 S4.1.4 compression pointer
// - Builder never emits pointers itself (see Builder.h), so this buffer
//   is assembled by hand: build one real question, then append a second
//   question whose name is just a pointer back at the first
// - the pointer resolves to the same labels as the name it points to

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Build the First Question Normally");

    Message oneQuestion;
    Question q1;
    q1.qname.labels.push_back("www");
    q1.qname.labels.push_back("example");
    q1.qname.labels.push_back("com");
    q1.qtype = 1;  // A
    q1.qclass = 1; // IN
    oneQuestion.questions.push_back(q1);

    Vector<std::byte> buffer;
    (void)Builder::build(oneQuestion, buffer);

    // The header is always the first 12 bytes, so q1's name starts
    // immediately after it, at offset 12.
    constexpr std::uint16_t nameOffset = 12;
    std::cout << "buffer size after question 1 : " << buffer.size() << " bytes\n\n";

    setTitle("Append a Second Question That Points at the First");

    // A compression pointer: top two bits set, low 14 bits are the offset.
    buffer.push_back(std::byte{static_cast<std::uint8_t>(0xC0 | (nameOffset >> 8))});
    buffer.push_back(std::byte{static_cast<std::uint8_t>(nameOffset & 0xFF)});
    buffer.push_back(std::byte{0});  // qtype high byte
    buffer.push_back(std::byte{28}); // qtype low byte (AAAA = 28)
    buffer.push_back(std::byte{0});  // qclass high byte
    buffer.push_back(std::byte{1});  // qclass low byte (IN)

    // Patch qdcount from 1 to 2 (bytes 4-5 of the header, big-endian).
    buffer[4] = std::byte{0};
    buffer[5] = std::byte{2};

    std::cout << "buffer size after question 2 : " << buffer.size() << " bytes\n\n";

    setTitle("Parse Both Questions Back");

    Message parsed;
    Status status =
        Parser::parse(std::span<const std::byte>(buffer.begin(), buffer.size()), parsed);
    std::cout << "parse() status  : " << static_cast<int>(status) << "\n";
    if (status != Status::OK)
        return;

    std::cout << "question count  : " << parsed.questions.size() << "\n\n";

    for (std::size_t i = 0; i < parsed.questions.size(); ++i) {
        const Name& name = parsed.questions[i].qname;
        std::cout << "question " << i << " qname : ";
        for (std::size_t j = 0; j < name.labels.size(); ++j) {
            if (j > 0)
                std::cout << ".";
            std::cout << name.labels[j];
        }
        std::cout << "  (qtype " << parsed.questions[i].qtype << ")\n";
    }
}

REGISTER_EXAMPLE_SUITE();
