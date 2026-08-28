/**
 * @file Parser.cpp
 * @brief DnsPro::Parser implementation.
 *
 * Contains the implementation of Parser's bytes-to-Message parsing,
 * including DNS name decompression.
 */

// ============================================================
// Implementation for DnsPro::Parser.
// ============================================================
//
//  Sections:
//   1. Public Entry Point
//   2. Header
//   3. Name (labels + compression pointers)
//   4. Question / Resource Record
//   5. Primitive Readers
//
// ============================================================

// clang-format off
#include <DnsPro/Parser.h> // Parser

#include <string> // std::string
// clang-format on

namespace DnsPro {

// ============================================================
//  Section 1 — Public Entry Point
// ============================================================
Status Parser::parse(std::span<const std::byte> buffer, Packet::Message& out) {
    std::size_t offset = 0;

    if (Status s = parseHeader(buffer, offset, out.header); s != Status::OK) return s;

    out.questions.clear();
    out.questions.reserve(out.header.qdcount);
    for (std::uint16_t i = 0; i < out.header.qdcount; ++i) {
        Packet::Question q;
        if (Status s = parseQuestion(buffer, offset, q); s != Status::OK) return s;
        out.questions.push_back(std::move(q));
    }

    out.answers.clear();
    out.answers.reserve(out.header.ancount);
    for (std::uint16_t i = 0; i < out.header.ancount; ++i) {
        Packet::ResourceRecord rr;
        if (Status s = parseResourceRecord(buffer, offset, rr); s != Status::OK) return s;
        out.answers.push_back(std::move(rr));
    }

    out.authorities.clear();
    out.authorities.reserve(out.header.nscount);
    for (std::uint16_t i = 0; i < out.header.nscount; ++i) {
        Packet::ResourceRecord rr;
        if (Status s = parseResourceRecord(buffer, offset, rr); s != Status::OK) return s;
        out.authorities.push_back(std::move(rr));
    }

    out.additionals.clear();
    out.additionals.reserve(out.header.arcount);
    for (std::uint16_t i = 0; i < out.header.arcount; ++i) {
        Packet::ResourceRecord rr;
        if (Status s = parseResourceRecord(buffer, offset, rr); s != Status::OK) return s;
        out.additionals.push_back(std::move(rr));
    }

    return Status::OK;
}


// ============================================================
//  Section 2 — Header
// ============================================================
Status Parser::parseHeader(std::span<const std::byte> buffer, std::size_t& offset,
                            Packet::Header& out) {
    std::uint16_t id;
    if (Status s = readU16(buffer, offset, id); s != Status::OK) return s;

    if (offset + 2 > buffer.size()) return Status::BUFFER_TOO_SMALL;
    auto flags1 = std::to_integer<std::uint8_t>(buffer[offset]);
    auto flags2 = std::to_integer<std::uint8_t>(buffer[offset + 1]);
    offset += 2;

    std::uint16_t qdcount, ancount, nscount, arcount;
    if (Status s = readU16(buffer, offset, qdcount); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, ancount); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, nscount); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, arcount); s != Status::OK) return s;

    out.id = id;

    // Wire layout (RFC 1035 S4.1.1), MSB first within each flags byte:
    //   byte1: QR(1) OPCODE(4) AA(1) TC(1) RD(1)
    //   byte2: RA(1) Z(3) RCODE(4)
    out.qr     = (flags1 >> 7) & 0x1;
    out.opcode = (flags1 >> 3) & 0xF;
    out.aa     = (flags1 >> 2) & 0x1;
    out.tc     = (flags1 >> 1) & 0x1;
    out.rd     =  flags1       & 0x1;

    out.ra    = (flags2 >> 7) & 0x1;
    out.z     = (flags2 >> 4) & 0x7;
    out.rcode =  flags2       & 0xF;

    out.qdcount = qdcount;
    out.ancount = ancount;
    out.nscount = nscount;
    out.arcount = arcount;

    return Status::OK;
}


// ============================================================
//  Section 3 — Name (labels + compression pointers)
// ============================================================
Status Parser::parseName(std::span<const std::byte> buffer, std::size_t& offset, Packet::Name& out) {
    out.labels.clear();

    std::size_t cursor = offset; // walks the name, possibly through pointers
    bool jumped = false;
    std::size_t resumeOffset = 0; // where `offset` lands once we're done, if we jumped

    // Bounds the total number of labels+pointer-hops this call may take.
    // pointerTarget < cursor (enforced below) already rules out a literal
    // loop, but a pathological packet could still chain O(buffer size)
    // strictly-decreasing pointers -- this caps the work regardless.
    std::size_t stepsRemaining = buffer.size() + 1;

    while (true) {
        if (stepsRemaining-- == 0) return Status::COMPRESSION_LOOP;
        if (cursor >= buffer.size()) return Status::BUFFER_TOO_SMALL;

        std::uint8_t lengthByte = std::to_integer<std::uint8_t>(buffer[cursor]);

        // Compression pointer: top two bits set (RFC 1035 S4.1.4).
        if ((lengthByte & 0xC0) == 0xC0) {
            if (cursor + 2 > buffer.size()) return Status::BUFFER_TOO_SMALL;

            auto lowByte = std::to_integer<std::uint8_t>(buffer[cursor + 1]);
            std::size_t target = (static_cast<std::size_t>(lengthByte & 0x3F) << 8) | lowByte;

            if (!jumped) {
                resumeOffset = cursor + 2; // caller's cursor resumes after the pointer itself
                jumped = true;
            }

            // Must point strictly backward. A forward or self pointer can't
            // occur in a well-formed message and would otherwise let a
            // malicious packet build an unbounded reference chain.
            if (target >= cursor) return Status::COMPRESSION_FORWARD_POINTER;

            cursor = target;
            continue;
        }

        // Terminating zero-length label: end of name.
        if (lengthByte == 0) {
            cursor += 1;
            break;
        }

        // Regular label. Top two bits must be 00 -- anything else (01, 10)
        // is a reserved/invalid length encoding in this RFC.
        if ((lengthByte & 0xC0) != 0x00) return Status::MALFORMED_NAME;

        std::size_t labelStart = cursor + 1;
        std::size_t labelLen = lengthByte;
        if (labelStart + labelLen > buffer.size()) return Status::BUFFER_TOO_SMALL;

        out.labels.push_back(
            std::string(reinterpret_cast<const char*>(buffer.data() + labelStart), labelLen));

        cursor = labelStart + labelLen;
    }

    offset = jumped ? resumeOffset : cursor;
    return Status::OK;
}


// ============================================================
//  Section 4 — Question / Resource Record
// ============================================================
Status Parser::parseQuestion(std::span<const std::byte> buffer, std::size_t& offset,
                              Packet::Question& out) {
    if (Status s = parseName(buffer, offset, out.qname); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, out.qtype); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, out.qclass); s != Status::OK) return s;
    return Status::OK;
}

Status Parser::parseResourceRecord(std::span<const std::byte> buffer, std::size_t& offset,
                                    Packet::ResourceRecord& out) {
    if (Status s = parseName(buffer, offset, out.name); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, out.type); s != Status::OK) return s;
    if (Status s = readU16(buffer, offset, out.rclass); s != Status::OK) return s;
    if (Status s = readU32(buffer, offset, out.ttl); s != Status::OK) return s;

    std::uint16_t rdlength;
    if (Status s = readU16(buffer, offset, rdlength); s != Status::OK) return s;

    if (offset + rdlength > buffer.size()) return Status::RDATA_LENGTH_MISMATCH;

    out.rdataOffset = offset; // Captured before the loop below advances offset past rdata --
                              // this is rdata's own start position in the original buffer,
                              // which a caller needs later to decode any compressed domain
                              // name embedded inside it (see ResourceRecord::rdataOffset).
    out.rdata.clear();
    out.rdata.reserve(rdlength);
    for (std::size_t i = 0; i < rdlength; ++i) {
        out.rdata.push_back(buffer[offset + i]);
    }
    offset += rdlength;

    return Status::OK;
}


// ============================================================
//  Section 5 — Primitive Readers
// ============================================================
Status Parser::readU16(std::span<const std::byte> buffer, std::size_t& offset, std::uint16_t& out) {
    if (offset + 2 > buffer.size()) return Status::BUFFER_TOO_SMALL;

    out = static_cast<std::uint16_t>((std::to_integer<std::uint16_t>(buffer[offset]) << 8) |
                                      std::to_integer<std::uint16_t>(buffer[offset + 1]));
    offset += 2;
    return Status::OK;
}

Status Parser::readU32(std::span<const std::byte> buffer, std::size_t& offset, std::uint32_t& out) {
    if (offset + 4 > buffer.size()) return Status::BUFFER_TOO_SMALL;

    out = (std::to_integer<std::uint32_t>(buffer[offset]) << 24) |
          (std::to_integer<std::uint32_t>(buffer[offset + 1]) << 16) |
          (std::to_integer<std::uint32_t>(buffer[offset + 2]) << 8) |
          std::to_integer<std::uint32_t>(buffer[offset + 3]);
    offset += 4;
    return Status::OK;
}

} // namespace DnsPro
