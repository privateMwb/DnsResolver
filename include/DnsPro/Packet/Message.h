/**
 * @file            Message.h
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
#include <DnsPro/Packet/Question.h>       // Question
#include <DnsPro/Packet/ResourceRecord.h> // ResourceRecord

#include <VectorPro/Vector.h> // VectorPro::Vector (section storage)
// clang-format on

// A full DNS message: the Header plus its four sections. This is what
// Parser::parse() populates and Builder::build() consumes -- Header,
// Question, and ResourceRecord on their own don't say how many of each a
// given packet has; Message is what ties a Header's count fields to actual
// parsed arrays.

namespace DnsPro::Packet {

using namespace VectorPro;

/**
 * @brief A complete DNS message (query or response).
 * @details `header`'s qdcount/ancount/nscount/arcount fields describe the
 * wire-format counts; the section vectors below hold the actual parsed
 * (or, for Builder, to-be-serialized) entries. Parser::parse() keeps both
 * in sync -- a mismatch is reported as `Status::SECTION_COUNT_MISMATCH`
 * rather than silently trusting one over the other.
 */
struct Message {
    Header header; ///< The 12-byte message header.

    Vector<Question> questions;         ///< Question section.
    Vector<ResourceRecord> answers;     ///< Answer section.
    Vector<ResourceRecord> authorities; ///< Authority section.
    Vector<ResourceRecord> additionals; ///< Additional section.
};

} // namespace DnsPro::Packet
