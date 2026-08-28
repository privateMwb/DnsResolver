// Custom-framework mirror of google_tests/suite/unit/rdata_name_compression.cpp --
// see that file for full coverage notes.

#include <support/framework.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kMxType = 15;
constexpr std::uint16_t kClass = 1; // IN
constexpr std::uint32_t kTtl = 300;
constexpr std::uint16_t kPreference = 10;
constexpr std::uint16_t kFirstNameOffset = 12;

Vector<std::byte> buildBaseQuery() {
    Packet::Question question;
    question.qname.labels.push_back("example");
    question.qname.labels.push_back("com");
    question.qtype = kMxType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

void pushU16(Vector<std::byte>& bytes, std::uint16_t value) {
    bytes.push_back(std::byte{static_cast<std::uint8_t>(value >> 8)});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(value & 0xFF)});
}

void pushU32(Vector<std::byte>& bytes, std::uint32_t value) {
    bytes.push_back(std::byte{static_cast<std::uint8_t>((value >> 24) & 0xFF)});
    bytes.push_back(std::byte{static_cast<std::uint8_t>((value >> 16) & 0xFF)});
    bytes.push_back(std::byte{static_cast<std::uint8_t>((value >> 8) & 0xFF)});
    bytes.push_back(std::byte{static_cast<std::uint8_t>(value & 0xFF)});
}

Vector<std::byte> appendMxAnswer(Vector<std::byte> bytes) {
    pushU16(bytes, static_cast<std::uint16_t>(0xC000 | kFirstNameOffset));
    pushU16(bytes, kMxType);
    pushU16(bytes, kClass);
    pushU32(bytes, kTtl);

    Vector<std::byte> rdata;
    pushU16(rdata, kPreference);
    rdata.push_back(std::byte{4});
    rdata.push_back(std::byte{'m'});
    rdata.push_back(std::byte{'a'});
    rdata.push_back(std::byte{'i'});
    rdata.push_back(std::byte{'l'});
    pushU16(rdata, static_cast<std::uint16_t>(0xC000 | kFirstNameOffset));

    pushU16(bytes, static_cast<std::uint16_t>(rdata.size()));
    for (std::size_t i = 0; i < rdata.size(); ++i)
        bytes.push_back(rdata[i]);

    bytes[6] = std::byte{0};
    bytes[7] = std::byte{1};

    return bytes;
}

void RdataOffsetLandsOnRdataStart() {
    Vector<std::byte> bytes = appendMxAnswer(buildBaseQuery());
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    CHK(s == Status::OK);
    CHK(out.answers.size() == 1);
    if (out.answers.size() != 1)
        return;

    const Packet::ResourceRecord& mx = out.answers[0];
    CHK(mx.rdata.size() == 9);
    CHK(mx.rdataOffset + mx.rdata.size() <= bytes.size());
    CHK(buffer[mx.rdataOffset] == mx.rdata[0]);

    auto preference =
        static_cast<std::uint16_t>((std::to_integer<std::uint16_t>(mx.rdata[0]) << 8) |
                                   std::to_integer<std::uint16_t>(mx.rdata[1]));
    CHK(preference == kPreference);
}

void ParseNameDecodesCompressedExchange() {
    Vector<std::byte> bytes = appendMxAnswer(buildBaseQuery());
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    CHK(s == Status::OK);
    CHK(out.answers.size() == 1);
    if (out.answers.size() != 1)
        return;

    std::size_t nameOffset = out.answers[0].rdataOffset + 2;
    Packet::Name exchange;
    Status nameStatus = Parser::parseName(buffer, nameOffset, exchange);

    CHK(nameStatus == Status::OK);
    CHK(exchange.labels.size() == 3);
    if (exchange.labels.size() != 3)
        return;
    CHK(exchange.labels[0] == "mail");
    CHK(exchange.labels[1] == "example");
    CHK(exchange.labels[2] == "com");
}

void run_tests() {
    RUN(RdataOffsetLandsOnRdataStart);
    RUN(ParseNameDecodesCompressedExchange);
}

REGISTER_TEST_SUITE()

} // namespace
