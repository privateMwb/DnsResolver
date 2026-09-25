// DnsResolver Message Move Benchmark Suite
// Measures Message's move constructor and move assignment operator
// separately.
//
// VectorPro::Vector's move operations are O(1) regardless of element
// count -- they steal the internal buffer pointer, size, and capacity
// rather than copying elements -- so Message's move cost doesn't
// depend on how many questions/answers it holds. That means the
// source doesn't need to be repopulated between iterations: the first
// call moves real data, every call after that moves an already-empty
// Message (left behind by the previous move), and both cost the same.
// No round-trip needed to keep state "populated" across iterations.
//
// Covers:
// - Message move constructor
// - Message move assignment operator

#include <DnsPro/DnsResolver.h>

#include <benchmark/benchmark.h>

using namespace DnsPro;

namespace {

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::ResourceRecord makeAnswer(std::uint8_t lastOctet) {
    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = 1;   // A
    record.rclass = 1; // IN
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{lastOctet});
    return record;
}

Packet::Message makePopulatedMessage() {
    Packet::Message message;
    message.header.qr = 1;

    Packet::Question question;
    question.qname = makeName("example", "com");
    question.qtype = 1;
    question.qclass = 1;
    message.questions.push_back(question);

    for (std::uint8_t i = 0; i < 4; ++i) {
        message.answers.push_back(makeAnswer(static_cast<std::uint8_t>(34 + i)));
    }

    return message;
}

} // namespace

// Measures the Message move constructor.
static void Message_MoveConstruct(benchmark::State& state) {
    Packet::Message source = makePopulatedMessage();

    for (auto _ : state) {
        Packet::Message temp(std::move(source));
        benchmark::DoNotOptimize(temp);
    }
}
BENCHMARK(Message_MoveConstruct);

// Measures Message move assignment.
static void Message_MoveAssign(benchmark::State& state) {
    Packet::Message destination = makePopulatedMessage();
    Packet::Message source = makePopulatedMessage();

    for (auto _ : state) {
        destination = std::move(source);
        benchmark::DoNotOptimize(destination);
    }
}
BENCHMARK(Message_MoveAssign);
