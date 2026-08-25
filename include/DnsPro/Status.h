/**
 * @file            Status.h
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

// Result codes shared by Parser::parse() and Builder::build(). Kept
// top-level (not under Packet/) since it's not a wire-format type -- it's
// the library's own error-reporting vocabulary.

namespace DnsPro {

/**
 * @brief Result code returned by parsing and building operations.
 */
enum class Status {
    // clang-format off
    OK,                           ///< Success.
    BUFFER_TOO_SMALL,             ///< Buffer ended before a fixed-size field could be read.
    MALFORMED_NAME,               ///< A label length byte or name encoding was invalid.
    COMPRESSION_LOOP,             ///< A compression pointer chain revisited an offset (would loop forever).
    COMPRESSION_FORWARD_POINTER,  ///< A compression pointer targeted an offset at or after itself.
    SECTION_COUNT_MISMATCH,       ///< Fewer records were present than the header's count fields claimed.
    RDATA_LENGTH_MISMATCH,        ///< A record's rdlength didn't match the bytes available (parsing), or
                                  ///< rdata exceeded the 16-bit rdlength field's range (building).
    LABEL_TOO_LONG,               ///< A label exceeded the 63-byte encoding limit (Builder).
    SECTION_TOO_LARGE,            ///< A section held more than 65535 entries, more than qdcount/ancount/
                                  ///< nscount/arcount can represent (Builder).
    // clang-format on
};

} // namespace DnsPro
