/**
 * @file            Name.h
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
#include <VectorPro/Vector.h> // VectorPro::Vector (label storage)

#include <string> // std::string
// clang-format on

// A parsed DNS domain name as an ordered list of labels, e.g.
// "www.example.com" -> {"www", "example", "com"}. Shared by Question and
// ResourceRecord, since both carry a name and Phase 2's label/compression
// parsing is identical either way.

namespace DnsPro::Packet {

using namespace VectorPro;

/// @brief Ordered label storage for a domain name.
using Labels = Vector<std::string>;

/**
 * @brief A parsed domain name.
 */
struct Name {
    Labels labels; ///< Labels in order, most-specific first.
};

} // namespace DnsPro::Packet
