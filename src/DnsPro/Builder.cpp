/**
 * @file Builder.cpp
 * @brief DnsPro::Builder implementation.
 *
 * Contains the implementation of Builder's Message-to-bytes serialization.
 */

// ============================================================
// Implementation for DnsPro::Builder.
// ============================================================
//
//  Sections:
//   1. Public Entry Point
//   2. Header
//   3. Name
//   4. Question / Resource Record
//   5. Primitive Writers
//
// ============================================================

#include <DnsPro/Builder.h> // Builder

namespace DnsPro {

// ============================================================
//  Section 1 — Public Entry Point
// ============================================================
Status Builder::build(const Packet::Message& message, Vector<std::byte>& out) {
    if (message.questions.size() > 0xFFFF || message.answers.size() > 0xFFFF ||
        message.authorities.size() > 0xFFFF || message.additionals.size() > 0xFFFF) {
        return Status::SECTION_TOO_LARGE;
    }

    out.clear();

    // Counts come from the section vectors' actual sizes, not from
    // message.header directly -- see build()'s doc comment.
    Packet::Header header = message.header;
    header.qdcount = static_cast<std::uint16_t>(message.questions.size());
    header.ancount = static_cast<std::uint16_t>(message.answers.size());
    header.nscount = static_cast<std::uint16_t>(message.authorities.size());
    header.arcount = static_cast<std::uint16_t>(message.additionals.size());

    writeHeader(header, out);

    for (const auto& q : message.questions) {
        if (Status s = writeQuestion(q, out); s != Status::OK) return s;
    }
    for (const auto& rr : message.answers) {
        if (Status s = writeResourceRecord(rr, out); s != Status::OK) return s;
    }
    for (const auto& rr : message.authorities) {
        if (Status s = writeResourceRecord(rr, out); s != Status::OK) return s;
    }
    for (const auto& rr : message.additionals) {
        if (Status s = writeResourceRecord(rr, out); s != Status::OK) return s;
    }

    return Status::OK;
}


// ============================================================
//  Section 2 — Header
// ============================================================
void Builder::writeHeader(const Packet::Header& header, Vector<std::byte>& out) {
    writeU16(header.id, out);

    // Wire layout (RFC 1035 S4.1.1), MSB first within each flags byte --
    // mirrors Parser::parseHeader's unpacking, in reverse.
    auto flags1 = static_cast<std::uint8_t>((header.qr << 7) | (header.opcode << 3) |
                                             (header.aa << 2) | (header.tc << 1) | header.rd);
    auto flags2 = static_cast<std::uint8_t>((header.ra << 7) | (header.z << 4) | header.rcode);
    out.push_back(std::byte{flags1});
    out.push_back(std::byte{flags2});

    writeU16(header.qdcount, out);
    writeU16(header.ancount, out);
    writeU16(header.nscount, out);
    writeU16(header.arcount, out);
}


// ============================================================
//  Section 3 — Name
// ============================================================
Status Builder::writeName(const Packet::Name& name, Vector<std::byte>& out) {
    for (const auto& label : name.labels) {
        if (label.size() > 63) return Status::LABEL_TOO_LONG;

        out.push_back(std::byte{static_cast<std::uint8_t>(label.size())});
        for (char c : label) {
            out.push_back(std::byte{static_cast<std::uint8_t>(c)});
        }
    }

    out.push_back(std::byte{0}); // terminating zero-length label
    return Status::OK;
}


// ============================================================
//  Section 4 — Question / Resource Record
// ============================================================
Status Builder::writeQuestion(const Packet::Question& question, Vector<std::byte>& out) {
    if (Status s = writeName(question.qname, out); s != Status::OK) return s;
    writeU16(question.qtype, out);
    writeU16(question.qclass, out);
    return Status::OK;
}

Status Builder::writeResourceRecord(const Packet::ResourceRecord& record, Vector<std::byte>& out) {
    if (Status s = writeName(record.name, out); s != Status::OK) return s;
    writeU16(record.type, out);
    writeU16(record.rclass, out);
    writeU32(record.ttl, out);

    if (record.rdata.size() > 0xFFFF) return Status::RDATA_LENGTH_MISMATCH;
    writeU16(static_cast<std::uint16_t>(record.rdata.size()), out);

    for (std::byte b : record.rdata) {
        out.push_back(b);
    }

    return Status::OK;
}


// ============================================================
//  Section 5 — Primitive Writers
// ============================================================
void Builder::writeU16(std::uint16_t value, Vector<std::byte>& out) {
    out.push_back(std::byte{static_cast<std::uint8_t>((value >> 8) & 0xFF)});
    out.push_back(std::byte{static_cast<std::uint8_t>(value & 0xFF)});
}

void Builder::writeU32(std::uint32_t value, Vector<std::byte>& out) {
    out.push_back(std::byte{static_cast<std::uint8_t>((value >> 24) & 0xFF)});
    out.push_back(std::byte{static_cast<std::uint8_t>((value >> 16) & 0xFF)});
    out.push_back(std::byte{static_cast<std::uint8_t>((value >> 8) & 0xFF)});
    out.push_back(std::byte{static_cast<std::uint8_t>(value & 0xFF)});
}

} // namespace DnsPro
