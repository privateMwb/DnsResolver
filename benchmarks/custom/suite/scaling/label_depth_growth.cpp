// DnsResolver Label Depth Growth Benchmark Suite
// Measures how Parser::parse() and Builder::build() cost changes as a
// name's label depth grows - "com" (shallow) versus a long chain of
// subdomains (deep). Parser/Builder don't expose parseName()/writeName()
// publicly, so this goes through parse()/build() on a question-only
// message, where name cost dominates since there's nothing else in it.
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - parse() cost at 2, 8, and 32 labels deep
// - build() cost at 2, 8, and 32 labels deep

#include <support/framework.h>

#include <iostream>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

// Builds a name `labelCount` labels deep: "label0.label1. ... .com".
Packet::Name makeDeepName(std::size_t labelCount) {
    Packet::Name name;
    for (std::size_t i = 0; i + 1 < labelCount; ++i) {
        name.labels.push_back("label" + std::to_string(i));
    }
    name.labels.push_back("com");
    return name;
}

Packet::Message makeMessageWithDepth(std::size_t labelCount) {
    Packet::Question question;
    question.qname = makeDeepName(labelCount);
    question.qtype = kType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);
    return message;
}

void benchParseAt(std::size_t labelCount, const char* label) {
    Packet::Message message = makeMessageWithDepth(labelCount);

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

void benchBuildAt(std::size_t labelCount, const char* label) {
    Packet::Message message = makeMessageWithDepth(labelCount);

    auto f = [&] {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        (void)s;
    };

    BENCH(label, f);
}

} // namespace

// Measures parse() cost at 2, 8, and 32 labels deep.
static void bench_parse_growth() {
    benchParseAt(2, "2 labels deep");
    std::cout << "\n";

    benchParseAt(8, "8 labels deep");
    std::cout << "\n";

    benchParseAt(32, "32 labels deep");
}

// Measures build() cost at 2, 8, and 32 labels deep.
static void bench_build_growth() {
    benchBuildAt(2, "2 labels deep");
    std::cout << "\n";

    benchBuildAt(8, "8 labels deep");
    std::cout << "\n";

    benchBuildAt(32, "32 labels deep");
}

// Executes all label-depth-growth benchmark cases.
static void run_benchmarks() {
    bench_parse_growth();
    std::cout << "\n";

    bench_build_growth();
}

REGISTER_BENCH_SUITE();
