// DnsResolver Builder Build Benchmark Suite
// Measures Builder::build() performance on the same two message shapes
// as parse.cpp - a question-only query, and a response carrying answer
// records - isolating serialization cost from any
// Parser/ZoneStore/Resolver pipeline overhead.
//
// Each input Message is built once outside the timed loop; every
// iteration then re-serializes the same Message into a fresh output
// buffer. Google Benchmark's own --benchmark_min_time / iteration count
// handles the repeat-until-stable behavior the custom suite's
// SMALL/MEDIUM/LARGE tiers cover by hand.
//
// Covers:
// - build() on a query with a single question, no answer records
// - build() on a response with a single question and several answer records

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN
constexpr std::size_t kAnswerCount = 4;

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

Packet::Message makeQueryMessage() {
    Packet::Message message;
    message.questions.push_back(makeQuestion());
    return message;
}

Packet::Message makeAnswerMessage() {
    Packet::Message message;
    message.header.qr = 1;
    message.questions.push_back(makeQuestion());
    for (std::size_t i = 0; i < kAnswerCount; ++i) {
        message.answers.push_back(makeAnswer(static_cast<std::uint8_t>(34 + i)));
    }
    return message;
}

} // namespace

// Measures build() on a query with a single question, no answers.
static void Build_QuestionOnly(benchmark::State& state) {
    Packet::Message message = makeQueryMessage();

    for (auto _ : state) {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(bytes);
    }
}
BENCHMARK(Build_QuestionOnly);

// Measures build() on a response carrying answer records.
static void Build_FourAnswerRecords(benchmark::State& state) {
    Packet::Message message = makeAnswerMessage();

    for (auto _ : state) {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(bytes);
    }
}
BENCHMARK(Build_FourAnswerRecords);
