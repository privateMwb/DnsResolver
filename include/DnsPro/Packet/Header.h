/**
 * @file            Header.h
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
#include <cstdint> // std::uint8_t, std::uint16_t
// clang-format on

// The 12-byte DNS message header (RFC 1035 S4.1.1). Bit-field layout below
// is for in-memory convenience only -- it is NOT wire-compatible (bit-field
// ordering is implementation-defined). Parser.h/Builder.cpp go field-by-field
// off the raw buffer rather than reinterpret_cast'ing onto this struct.

namespace DnsPro::Packet {

/**
 * @brief The 12-byte DNS message header (RFC 1035 S4.1.1).
 */
struct Header {
    std::uint16_t id; ///< Query/response identifier, echoed back by the resolver.

    std::uint8_t qr : 1;     ///< 0 = query, 1 = response.
    std::uint8_t opcode : 4; ///< Kind of query (0 = standard).
    std::uint8_t aa : 1;     ///< Authoritative Answer.
    std::uint8_t tc : 1;     ///< Truncated.
    std::uint8_t rd : 1;     ///< Recursion Desired.

    std::uint8_t ra : 1;    ///< Recursion Available.
    std::uint8_t z : 3;     ///< Reserved, must be zero.
    std::uint8_t rcode : 4; ///< Response code (0 = no error).

    std::uint16_t qdcount; ///< Number of entries in the question section.
    std::uint16_t ancount; ///< Number of resource records in the answer section.
    std::uint16_t nscount; ///< Number of name server records in the authority section.
    std::uint16_t arcount; ///< Number of resource records in the additional section.
};

} // namespace DnsPro::Packet
