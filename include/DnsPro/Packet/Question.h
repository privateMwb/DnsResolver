/**
 * @file            Question.h
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

#include <cstdint> // std::uint16_t
// clang-format on

// A single entry in the question section (RFC 1035 S4.1.2).

namespace DnsPro::Packet {

/**
 * @brief A single entry in the question section (RFC 1035 S4.1.2).
 */
struct Question {
    Name qname;           ///< Domain name being queried.
    std::uint16_t qtype;  ///< Record type requested (A, AAAA, MX, ...).
    std::uint16_t qclass; ///< Query class (almost always IN = 1).
};

} // namespace DnsPro::Packet
