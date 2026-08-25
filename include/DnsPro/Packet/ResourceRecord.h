/**
 * @file            ResourceRecord.h
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
#include <DnsPro/Packet/Name.h> // Name

#include <VectorPro/Vector.h> // VectorPro::Vector (rdata storage)

#include <cstddef> // std::byte
#include <cstdint> // std::uint16_t, std::uint32_t
// clang-format on

// A single resource record, as found in the answer, authority, and
// additional sections (RFC 1035 S4.1.3).

namespace DnsPro::Packet {

using namespace VectorPro;

/**
 * @brief A single resource record.
 */
struct ResourceRecord {
    Name name;               ///< Owner name this record pertains to.
    std::uint16_t type;      ///< Record type (A, AAAA, MX, ...).
    std::uint16_t rclass;    ///< Record class (almost always IN = 1). Named
                             ///< `rclass`, not `class`, since the latter is a keyword.
    std::uint32_t ttl;       ///< Time-to-live, in seconds.
    Vector<std::byte> rdata; ///< Record data. `rdlength` is implicit: rdata.size().
};

} // namespace DnsPro::Packet
