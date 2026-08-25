// DnsResolver Name Parse Benchmark Suite
// Measures name-parsing cost, uncompressed versus through a compression
// pointer. Parser doesn't expose parseName() publicly, so both cases go
// through parse() on a minimal message where name cost dominates.
//
// There's no build-side counterpart to the compressed case: Builder
// never emits compression pointers (see Builder.h's doc comment), so a
// compressed message has to be hand-assembled here rather than produced
// through the library's own API -- Builder::build() writes the first
// question normally, then a second question's compression pointer and
// qtype/qclass are appended by hand, and qdcount is patched from 1 to 2
// directly in the byte buffer.
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - parse() on a name with no compression
// - parse() on a name reached through a compression pointer

#include <support/framework.h>

#include <iostream>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

// The first question's name always starts right here: Builder writes
// the fixed 12-byte header first, unconditionally.
constexpr std::uint16_t kFirstNameOffset = 12;

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::Question makeQuestion() {
    Packet::Question question;
    question.qname = makeName("example", "com");
    question.qtype = kType;
    question.qclass = kClass;
    return question;
}

Vector<std::byte> buildUncompressedQuery() {
    Packet::Message message;
    message.questions.push_back(makeQuestion());

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

Vector<std::byte> buildCompressedQuery() {
    // Start from a normal single-question message.
    Vector<std::byte> bytes = buildUncompressedQuery();

    // Append a second question whose qname is a compression pointer
    // back to the first question's name, instead of a real name.
    bytes.push_back(std::byte{static_cast<std::uint8_t>(0xC0 | (kFirstNameOffset >> 8))});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kFirstNameOffset & 0xFF)});
    bytes.push_back(std::byte{0});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kType)});
    bytes.push_back(std::byte{0});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(kClass)});

    // Patch qdcount from 1 to 2 -- big-endian u16 at byte offset 4-5,
    // right after the 2-byte id field.
    bytes[4] = std::byte{0};
    bytes[5] = std::byte{2};

    return bytes;
}

} // namespace

// Measures parse() on a name with no compression.
static void bench_parse_uncompressed() {
    Vector<std::byte> bytes = buildUncompressedQuery();
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    auto f = [&] {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        (void)s;
    };

    BENCH("Uncompressed name", f);
}

// Measures parse() on a name reached through a compression pointer.
static void bench_parse_compressed() {
    Vector<std::byte> bytes = buildCompressedQuery();
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    auto f = [&] {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        (void)s;
    };

    BENCH("Name via compression pointer", f);
}

// Executes all name-parse benchmark cases.
static void run_benchmarks() {
    bench_parse_uncompressed();
    std::cout << "\n";

    bench_parse_compressed();
}

REGISTER_BENCH_SUITE();
