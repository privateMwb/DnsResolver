// DnsResolver Label Depth Growth Benchmark Suite
// Measures how Parser::parse() and Builder::build() cost changes as a
// name's label depth grows - "com" (shallow) versus a long chain of
// subdomains (deep). Parser/Builder don't expose parseName()/writeName()
// publicly, so this goes through parse()/build() on a question-only
// message, where name cost dominates since there's nothing else in it.
//
// Covers:
// - parse() cost at 2, 8, and 32 labels deep
// - build() cost at 2, 8, and 32 labels deep

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

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

// Measures parse() cost at a given label depth.
void ParseAt(benchmark::State& state, std::size_t labelCount) {
    Packet::Message message = makeMessageWithDepth(labelCount);

    Vector<std::byte> bytes;
    Status buildStatus = Builder::build(message, bytes);
    benchmark::DoNotOptimize(buildStatus);
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    for (auto _ : state) {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(out);
    }
}

// Measures build() cost at a given label depth.
void BuildAt(benchmark::State& state, std::size_t labelCount) {
    Packet::Message message = makeMessageWithDepth(labelCount);

    for (auto _ : state) {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(bytes);
    }
}

} // namespace

BENCHMARK_CAPTURE(ParseAt, TwoLabelsDeep, 2);
BENCHMARK_CAPTURE(ParseAt, EightLabelsDeep, 8);
BENCHMARK_CAPTURE(ParseAt, ThirtyTwoLabelsDeep, 32);

BENCHMARK_CAPTURE(BuildAt, TwoLabelsDeep, 2);
BENCHMARK_CAPTURE(BuildAt, EightLabelsDeep, 8);
BENCHMARK_CAPTURE(BuildAt, ThirtyTwoLabelsDeep, 32);
