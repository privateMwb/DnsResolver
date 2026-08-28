// Covers the capability added for ResourceRecord::rdataOffset and the
// now-public Parser::parseName(): decoding a compressed domain name
// embedded inside a record's rdata (e.g. an MX record's exchange, an NS
// record's nsdname) -- something Parser::parse() itself never attempts,
// since it has no way to know, for an arbitrary record type, that rdata
// contains a name at all.
//
// Coverage:
// - rdataOffset lands exactly where rdata's bytes actually start
// - Parser::parseName(), called externally with that offset, correctly
//   decodes a name that is part literal label ("mail") and part
//   compression pointer (back to the question's qname) -- the common
//   real-world shape for an MX exchange
//
// Builder has no rdata-aware name support (rdata is opaque bytes to
// it too), so this record is hand-assembled the same way
// compression.cpp hand-assembles its pointer-bearing question.

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kMxType = 15;
constexpr std::uint16_t kClass = 1; // IN
constexpr std::uint32_t kTtl = 300;
constexpr std::uint16_t kPreference = 10;

// The first question's name always starts right here: Builder writes
// the fixed 12-byte header first, unconditionally.
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

// Appends one hand-built MX answer record to `bytes`:
//   name    -- compression pointer back to the question's qname
//   type    -- MX (15)
//   class   -- IN (1)
//   ttl     -- kTtl
//   rdata   -- 2-byte preference, then an exchange name that's part
//              literal ("mail") and part compression pointer (back to
//              the same question qname) -- e.g. "mail.example.com"
//              where only ".example.com" is shared/compressed, the
//              realistic shape for a real MX record.
// Also patches the header's ancount from 0 to 1.
Vector<std::byte> appendMxAnswer(Vector<std::byte> bytes) {
    pushU16(bytes, static_cast<std::uint16_t>(0xC000 | kFirstNameOffset)); // name: pointer to qname
    pushU16(bytes, kMxType);
    pushU16(bytes, kClass);
    pushU32(bytes, kTtl);

    Vector<std::byte> rdata;
    pushU16(rdata, kPreference);
    rdata.push_back(std::byte{4}); // label "mail", length 4
    rdata.push_back(std::byte{'m'});
    rdata.push_back(std::byte{'a'});
    rdata.push_back(std::byte{'i'});
    rdata.push_back(std::byte{'l'});
    pushU16(rdata, static_cast<std::uint16_t>(0xC000 | kFirstNameOffset)); // pointer to qname

    pushU16(bytes, static_cast<std::uint16_t>(rdata.size())); // rdlength
    for (std::size_t i = 0; i < rdata.size(); ++i)
        bytes.push_back(rdata[i]);

    bytes[6] = std::byte{0}; // ancount high byte
    bytes[7] = std::byte{1}; // ancount low byte

    return bytes;
}

} // namespace

// Verifies rdataOffset points exactly at rdata's first byte, and that
// the preference field (a plain, uncompressed 2-byte value at the very
// start of rdata) reads back correctly using it.
TEST(RdataNameCompression, RdataOffsetLandsOnRdataStart) {
    Vector<std::byte> bytes = appendMxAnswer(buildBaseQuery());
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    ASSERT_EQ(s, Status::OK);
    ASSERT_EQ(out.answers.size(), 1);

    const Packet::ResourceRecord& mx = out.answers[0];
    ASSERT_EQ(mx.rdata.size(), 9); // 2 (preference) + 1 (len) + 4 ("mail") + 2 (pointer)
    ASSERT_LE(mx.rdataOffset + mx.rdata.size(), bytes.size());

    // The byte at rdataOffset in the original buffer must be the same
    // byte as rdata[0] -- confirms the offset isn't off-by-N in either
    // direction, not just "close enough".
    EXPECT_EQ(buffer[mx.rdataOffset], mx.rdata[0]);

    auto preference =
        static_cast<std::uint16_t>((std::to_integer<std::uint16_t>(mx.rdata[0]) << 8) |
                                   std::to_integer<std::uint16_t>(mx.rdata[1]));
    EXPECT_EQ(preference, kPreference);
}

// Verifies the actual point of this capability: calling the now-public
// Parser::parseName() at rdataOffset (skipping the 2-byte preference
// field first) correctly decodes an exchange name that's part literal
// label, part compression pointer back into the message.
TEST(RdataNameCompression, ParseNameDecodesCompressedExchange) {
    Vector<std::byte> bytes = appendMxAnswer(buildBaseQuery());
    std::span<const std::byte> buffer(bytes.begin(), bytes.size());

    Packet::Message out;
    Status s = Parser::parse(buffer, out);
    ASSERT_EQ(s, Status::OK);
    ASSERT_EQ(out.answers.size(), 1);

    std::size_t nameOffset = out.answers[0].rdataOffset + 2; // past the 2-byte preference
    Packet::Name exchange;
    Status nameStatus = Parser::parseName(buffer, nameOffset, exchange);

    ASSERT_EQ(nameStatus, Status::OK);
    ASSERT_EQ(exchange.labels.size(), 3);
    EXPECT_EQ(exchange.labels[0], "mail");
    EXPECT_EQ(exchange.labels[1], "example");
    EXPECT_EQ(exchange.labels[2], "com");
}
