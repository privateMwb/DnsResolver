/**
 * @file            DnsResolver.h
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
#include <DnsPro/Packet/Name.h>           // Name, Labels
#include <DnsPro/Packet/Question.h>       // Question
#include <DnsPro/Packet/ResourceRecord.h> // ResourceRecord
#include <DnsPro/Status.h>                // Status
#include <DnsPro/Parser.h>                // Parser
#include <DnsPro/Builder.h>               // Builder
#include <DnsPro/ZoneStore.h>             // ZoneStore
#include <DnsPro/Resolver.h>              // Resolver
// clang-format on

// Phase 1-5 complete: packet structs, parsing, building, zone storage,
// resolution.

namespace DnsPro {

// Pulls DnsPro::Packet's members up into DnsPro, so `DnsPro::Header` (and
// the `rain::` alias below, which only reaches direct DnsPro members) both
// resolve without callers having to spell out `DnsPro::Packet::Header`.
using namespace Packet;

} // namespace DnsPro

/// @brief Umbrella alias so this library's types are reachable as
/// `rain::Type`, alongside every other project library, while its true
/// namespace remains `DnsPro` (with packet structs nested under
/// `DnsPro::Packet`). Reopens `rain` rather than aliasing it, since
/// multiple libraries each contribute their own names into the same `rain`
/// namespace -- an alias (`namespace rain = DnsPro;`) can only ever bind to
/// one target and collides the moment a second library declares its own
/// `rain` alias to something else. Declared here only (DnsResolver's main
/// header); no other DnsResolver header redeclares this.
namespace rain {
using namespace DnsPro;
}
