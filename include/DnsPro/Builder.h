/**
 * @file            Builder.h
 *
 * @date            2026-8-22
 *
 * @version         1.0.0
 *
 * @copyright       Copyright (c) 2026 privateMwb
 *                  All rights reserved.
 *
 * @attention       This source is released under the MIT license
 *                  SPDX-License-Identifier: MIT
 *                  <http://opensource.org/licenses/MIT>
 */

#pragma once

// clang-format off
#include <DnsPro/Packet/Header.h>         // Header
#include <DnsPro/Packet/Message.h>        // Message
#include <DnsPro/Packet/Name.h>           // Name
#include <DnsPro/Packet/Question.h>       // Question
#include <DnsPro/Packet/ResourceRecord.h> // ResourceRecord
#include <DnsPro/Status.h>                // Status

#include <VectorPro/Vector.h> // VectorPro::Vector (output buffer)

#include <cstddef> // std::byte
#include <cstdint> // std::uint16_t, std::uint32_t
// clang-format on

// Structures -> bytes. The reverse of Parser. Stateless (every member is
// static), same as Parser. Names are always written in full, uncompressed
// form -- Builder never emits compression pointers. Matching an arbitrary
// original encoder's compression choices byte-for-byte isn't a correctness
// requirement (compression is a size optimization, not part of the
// semantics), and it's a meaningfully harder problem (suffix matching
// against everything already written); Phase 6's round-trip tests check
// that a rebuilt packet re-parses to the same Message, not that it's
// byte-identical to whatever produced the original.

namespace DnsPro {

using namespace VectorPro;

/**
 * @brief Serializes a Message into raw DNS wire-format bytes.
 * @details header.qdcount/ancount/nscount/arcount are ignored on input --
 * see build() for why counts are derived from the section vectors instead.
 */
class Builder {
  public:
    Builder() = delete;

    /**
     * @brief Serializes `message` into `out`.
     * @param message Message to serialize.
     * @param out Buffer to append the serialized bytes to. Cleared first.
     * @return `Status::OK` on success; `Status::SECTION_TOO_LARGE` if any
     * section holds more than 65535 entries; `Status::LABEL_TOO_LONG` or
     * `Status::RDATA_LENGTH_MISMATCH` if a name or record's rdata can't be
     * represented in wire format.
     * @details `out`'s section counts come from `message.questions.size()`
     * etc., not from `message.header`'s count fields -- so a caller who
     * appends to a section vector without separately updating the
     * matching header count still gets a header that accurately describes
     * what follows it.
     */
    [[nodiscard]] static Status build(const Packet::Message& message, Vector<std::byte>& out);

  private:
    /// @brief Writes the fixed 12-byte header. Cannot fail -- every field is fixed-size.
    static void writeHeader(const Packet::Header& header, Vector<std::byte>& out);

    /**
     * @brief Writes a domain name as a sequence of length-prefixed labels
     * followed by a terminating zero-length label. Never emits a
     * compression pointer.
     * @return `Status::OK` on success, `Status::LABEL_TOO_LONG` if any
     * label exceeds 63 bytes.
     */
    [[nodiscard]] static Status writeName(const Packet::Name& name, Vector<std::byte>& out);

    /// @brief Writes one question-section entry (name, qtype, qclass).
    [[nodiscard]] static Status writeQuestion(const Packet::Question& question,
                                              Vector<std::byte>& out);

    /**
     * @brief Writes one resource record (name, type, class, ttl, rdlength, rdata).
     * @return `Status::OK` on success; `Status::LABEL_TOO_LONG` from the
     * name; `Status::RDATA_LENGTH_MISMATCH` if `rdata.size()` exceeds
     * what the 16-bit rdlength field can represent.
     */
    [[nodiscard]] static Status writeResourceRecord(const Packet::ResourceRecord& record,
                                                    Vector<std::byte>& out);

    /// @brief Appends `value` as two big-endian bytes.
    static void writeU16(std::uint16_t value, Vector<std::byte>& out);
    /// @brief Appends `value` as four big-endian bytes.
    static void writeU32(std::uint32_t value, Vector<std::byte>& out);
};

} // namespace DnsPro
