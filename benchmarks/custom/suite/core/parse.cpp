// DnsResolver Parser Parse Benchmark Suite
// Measures Parser::parse() performance on two representative message
// shapes - a query with just a question section, and a response
// carrying answer records alongside it - isolating parsing cost from
// any Builder/ZoneStore/Resolver pipeline overhead.
//
// Each input buffer is built once (via Builder::build(), itself
// unrelated to what's being timed) outside the timed section; every
// iteration then re-parses the same buffer, matching the
// SMALL/MEDIUM/LARGE iteration tiers this suite runs at (see the suite
// README).
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - parse() on a query with a single question, no answer records
// - parse() on a response with a single question and several answer records

#include <support/framework.h>

#include <iostream>

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
static void bench_parse_question() {
    Vector<std::byte> bytes = buildQuery();
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    auto f = [&] {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        (void)s;
    };

    BENCH("Query, single question", f);
}

// Measures parse() on a response carrying answer records.
static void bench_parse_answer() {
    Vector<std::byte> bytes = buildAnswerMessage();
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    auto f = [&] {
        Packet::Message out;
        Status s = Parser::parse(buffer, out);
        (void)s;
    };

    BENCH("Response, 4 answer records", f);
}

// Executes all parse benchmark cases.
static void run_benchmarks() {
    bench_parse_question();
    std::cout << "\n";

    bench_parse_answer();
}

REGISTER_BENCH_SUITE();
