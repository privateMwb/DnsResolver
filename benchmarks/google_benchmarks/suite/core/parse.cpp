// DnsResolver Parser Parse Benchmark Suite
// Measures Parser::parse() performance on two representative message
// shapes - a query with just a question section, and a response
// carrying answer records alongside it - isolating parsing cost from
// any Builder/ZoneStore/Resolver pipeline overhead.
//
// Each input buffer is built once (via Builder::build(), itself
// unrelated to what's being timed) outside the timed loop; every
// iteration then re-parses the same buffer. Google Benchmark's own
// --benchmark_min_time / iteration count handles the repeat-until-stable
// behavior the custom suite's SMALL/MEDIUM/LARGE tiers cover by hand.
//
// Covers:
// - parse() on a query with a single question, no answer records
// - parse() on a response with a single question and several answer records

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

Vector<std::byte> buildQuery() {
    Packet::Message message;
    message.questions.push_back(makeQuestion());

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

Vector<std::byte> buildAnswerMessage() {
    Packet::Message message;
    message.header.qr = 1;
    message.questions.push_back(makeQuestion());
    for (std::size_t i = 0; i < kAnswerCount; ++i) {
        message.answers.push_back(makeAnswer(static_cast<std::uint8_t>(34 + i)));
    }

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

} // namespace

// Measures parse() on a query with a single question, no answers.
static void Parse_QuestionOnly(benchmark::State& state) {
    Vector<std::byte> bytes = buildQuery();
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    for (auto _ : state) {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(Parse_QuestionOnly);

// Measures parse() on a response carrying answer records.
static void Parse_FourAnswerRecords(benchmark::State& state) {
    Vector<std::byte> bytes = buildAnswerMessage();
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    for (auto _ : state) {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        benchmark::DoNotOptimize(s);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(Parse_FourAnswerRecords);
