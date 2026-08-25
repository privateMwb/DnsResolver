// DnsResolver Builder Build Benchmark Suite
// Measures Builder::build() performance on the same two message shapes
// as parse.cpp - a question-only query, and a response carrying answer
// records - isolating serialization cost from any
// Parser/ZoneStore/Resolver pipeline overhead.
//
// Each input Message is built once outside the timed section; every
// iteration then re-serializes the same Message into a fresh output
// buffer, matching the SMALL/MEDIUM/LARGE iteration tiers this suite
// runs at (see the suite README).
//
// NOTE: no comparison baseline exists for DnsResolver (see the suite
// README) - this runs through BENCH_SOLO() rather than BENCH(), timing
// DnsResolver alone.
//
// Covers:
// - build() on a query with a single question, no answer records
// - build() on a response with a single question and several answer records

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
static void bench_build_query() {
    Packet::Message message = makeQueryMessage();

    auto f = [&] {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        (void)s;
    };

    BENCH("Query, single question", f);
}

// Measures build() on a response carrying answer records.
static void bench_build_answer() {
    Packet::Message message = makeAnswerMessage();

    auto f = [&] {
        Vector<std::byte> bytes;
        Status s = Builder::build(message, bytes);
        (void)s;
    };

    BENCH("Response, 4 answer records", f);
}

// Executes all build benchmark cases.
static void run_benchmarks() {
    bench_build_query();
    std::cout << "\n";

    bench_build_answer();
}

REGISTER_BENCH_SUITE();
