// DnsResolver Answer Count Growth Benchmark Suite
// Measures how Parser::parse() and Builder::build() cost changes as
// the number of answer records in a message grows - a different axis
// from parse.cpp/build.cpp in Core, which fix the answer count and
// vary call volume via the SMALL/MEDIUM/LARGE iteration tiers instead.
// Here the structural size (answer count) is what varies, via one
// BENCH() call per size rather than one call repeated more times.
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - parse() cost at 4, 16, and 64 answer records
// - build() cost at 4, 16, and 64 answer records

#include <support/framework.h>

#include <iostream>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

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

Packet::ResourceRecord makeAnswer(std::uint8_t lastOctet) {
    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{lastOctet});
    return record;
}

Packet::Message makeMessageWithAnswers(std::size_t answerCount) {
    Packet::Message message;
    message.header.qr = 1;
    message.questions.push_back(makeQuestion());
    for (std::size_t i = 0; i < answerCount; ++i) {
        message.answers.push_back(makeAnswer(static_cast<std::uint8_t>(i % 256)));
    }
    return message;
}

void benchParseAt(std::size_t answerCount, const char* label) {
    Packet::Message message = makeMessageWithAnswers(answerCount);

    Vector<std::byte> bytes;
    Status buildStatus = Builder::build(message, bytes);
    (void)buildStatus;
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    auto f = [&] {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        (void)s;
    };

    BENCH(label, f);
}

void benchBuildAt(std::size_t answerCount, const char* label) {
    Packet::Message message = makeMessageWithAnswers(answerCount);

    auto f = [&] {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        (void)s;
    };

    BENCH(label, f);
}

} // namespace

// Measures parse() cost at 4, 16, and 64 answer records.
static void bench_parse_growth() {
    benchParseAt(4, "4 answer records");
    std::cout << "\n";

    benchParseAt(16, "16 answer records");
    std::cout << "\n";

    benchParseAt(64, "64 answer records");
}

// Measures build() cost at 4, 16, and 64 answer records.
static void bench_build_growth() {
    benchBuildAt(4, "4 answer records");
    std::cout << "\n";

    benchBuildAt(16, "16 answer records");
    std::cout << "\n";

    benchBuildAt(64, "64 answer records");
}

// Executes all answer-count-growth benchmark cases.
static void run_benchmarks() {
    bench_parse_growth();
    std::cout << "\n";

    bench_build_growth();
}

REGISTER_BENCH_SUITE();
