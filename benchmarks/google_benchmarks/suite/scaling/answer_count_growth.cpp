// DnsResolver Answer Count Growth Benchmark Suite
// Measures how Parser::parse() and Builder::build() cost changes as
// the number of answer records in a message grows - a different axis
// from parse.cpp/build.cpp, which fix the answer count and vary call
// volume via Google Benchmark's own iteration count instead. Here the
// structural size (answer count) is what varies, via one
// BENCHMARK_CAPTURE registration per size rather than one benchmark
// repeated more times.
//
// Covers:
// - parse() cost at 4, 16, and 64 answer records
// - build() cost at 4, 16, and 64 answer records

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

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

// Measures parse() cost at a given answer count.
void ParseAt(benchmark::State& state, std::size_t answerCount) {
    Packet::Message message = makeMessageWithAnswers(answerCount);

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

// Measures build() cost at a given answer count.
void BuildAt(benchmark::State& state, std::size_t answerCount) {
    Packet::Message message = makeMessageWithAnswers(answerCount);

    for (auto _ : state) {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(bytes);
    }
}

} // namespace

BENCHMARK_CAPTURE(ParseAt, FourAnswerRecords, 4);
BENCHMARK_CAPTURE(ParseAt, SixteenAnswerRecords, 16);
BENCHMARK_CAPTURE(ParseAt, SixtyFourAnswerRecords, 64);

BENCHMARK_CAPTURE(BuildAt, FourAnswerRecords, 4);
BENCHMARK_CAPTURE(BuildAt, SixteenAnswerRecords, 16);
BENCHMARK_CAPTURE(BuildAt, SixtyFourAnswerRecords, 64);
