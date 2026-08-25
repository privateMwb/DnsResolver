/**
 * @file            Parser.h
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

#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint16_t, std::uint32_t
#include <span>    // std::span
// clang-format on

// Bytes -> structures. Stateless by design (every member is static) --
// nothing here needs to persist between parse() calls, and a raw buffer +
// cursor offset is all any of the internal steps need. Name parsing is kept
// separate from the rest since compression pointers (RFC 1035 S4.1.4) make
// it the one part of the format that isn't a flat, linear read.

namespace DnsPro {

/**
 * @brief Parses a raw DNS wire-format buffer into a Message.
 * @details All parsing is offset-driven: each step advances a cursor into
 * the original buffer and never copies it, so a single buffer is walked
 * once, start to finish, header through additional section. Compression
 * pointers in names are followed through the *original* buffer (not
 * copied out first) with a strictly-decreasing-offset check to guarantee
 * termination -- see `Status::COMPRESSION_FORWARD_POINTER` and
 * `Status::COMPRESSION_LOOP`.
 */
class Parser {
  public:
    Parser() = delete;

    /**
     * @brief Parses `buffer` into `out`.
     * @param buffer Raw DNS message bytes, as received off the wire.
     * @param out Message to populate. Left in an unspecified state on
     * failure -- callers should not use `out` unless `Status::OK` is returned.
     * @return `Status::OK` on success, or the first error encountered
     * while walking the buffer.
     */
    [[nodiscard]] static Status parse(std::span<const std::byte> buffer, Packet::Message& out);

  private:
    /// @brief Parses the fixed 12-byte header starting at `offset`.
    [[nodiscard]] static Status parseHeader(std::span<const std::byte> buffer, std::size_t& offset,
                                            Packet::Header& out);

    /**
     * @brief Parses a domain name starting at `offset`, following
     * compression pointers as needed.
     * @param offset Advanced past the name's own encoding in `buffer`
     * (i.e. past the pointer itself if compressed, or past the terminating
     * zero-length label otherwise). Not advanced into a followed pointer's
     * target -- that's a separate cursor internally.
     */
    [[nodiscard]] static Status parseName(std::span<const std::byte> buffer, std::size_t& offset,
                                          Packet::Name& out);

    /// @brief Parses one question-section entry (name, qtype, qclass) starting at `offset`.
    [[nodiscard]] static Status parseQuestion(std::span<const std::byte> buffer,
                                              std::size_t& offset, Packet::Question& out);

    /// @brief Parses one resource record (name, type, class, ttl, rdata) starting at `offset`.
    [[nodiscard]] static Status parseResourceRecord(std::span<const std::byte> buffer,
                                                    std::size_t& offset,
                                                    Packet::ResourceRecord& out);

    /// @brief Reads a big-endian `std::uint16_t` at `offset` and advances it by 2.
    [[nodiscard]] static Status readU16(std::span<const std::byte> buffer, std::size_t& offset,
                                        std::uint16_t& out);

    /// @brief Reads a big-endian `std::uint32_t` at `offset` and advances it by 4.
    [[nodiscard]] static Status readU32(std::span<const std::byte> buffer, std::size_t& offset,
                                        std::uint32_t& out);
};

} // namespace DnsPro
