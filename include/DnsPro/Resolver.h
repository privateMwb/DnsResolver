/**
 * @file            Resolver.h
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
#include <DnsPro/Builder.h>         // Builder
#include <DnsPro/Packet/Message.h>  // Message
#include <DnsPro/Parser.h>          // Parser
#include <DnsPro/Status.h>          // Status
#include <DnsPro/ZoneStore.h>       // ZoneStore

#include <VectorPro/Vector.h> // VectorPro::Vector

#include <cstddef> // std::byte
#include <cstdint> // std::uint8_t
#include <span>    // std::span
// clang-format on

// Ties Parser + ZoneStore + Builder together: raw query bytes in, raw
// response bytes out. Authoritative-only -- there's no upstream/recursive
// lookup path, so every answer comes straight from the ZoneStore passed
// in at construction, and RA (recursion available) is always 0.

namespace DnsPro {

using namespace VectorPro;

/**
 * @brief Resolves a raw DNS query against a ZoneStore, producing a raw
 * DNS response.
 */
class Resolver {
  private:
    ZoneStore& zone_; ///< Not owned. Caller's ZoneStore must outlive this Resolver.

    // RFC 1035 S4.1.1 response codes this resolver assigns. Not the full
    // RCODE space -- just what an authoritative-only resolver needs.
    static constexpr std::uint8_t RCODE_NOERROR = 0;  ///< Success (including NODATA).
    static constexpr std::uint8_t RCODE_FORMERR = 1;  ///< Query itself was malformed.
    static constexpr std::uint8_t RCODE_NXDOMAIN = 3; ///< Queried name doesn't exist.

  public:
    /**
     * @brief Constructs a Resolver over `zone`.
     * @param zone Zone data to answer queries from. Referenced, not
     * copied or owned -- must outlive this Resolver.
     */
    explicit Resolver(ZoneStore& zone) noexcept;

    /**
     * @brief Resolves a raw query.
     * @param query Raw DNS query bytes, as received off the wire.
     * @param response Populated with the raw DNS response bytes on
     * success. Left unspecified if this returns anything other than
     * `Status::OK`.
     * @return `Status::OK` if a response was built (regardless of the
     * response's own RCODE -- NXDOMAIN and NODATA are both `Status::OK`
     * here, since the resolver itself succeeded at producing an answer).
     * Anything else is a `Parser::parse()` failure on `query` itself; the
     * caller decides whether to drop the packet or synthesize a FORMERR
     * reply, since doing so needs the transport layer this library
     * doesn't own.
     * @details Only the first question in `query` is answered, echoed
     * back as the sole entry in the response's question section --
     * multi-question queries are handled the same way by most real-world
     * resolvers, since the format doesn't actually specify how a server
     * should combine answers to more than one.
     */
    [[nodiscard]] Status resolve(std::span<const std::byte> query,
                                 Vector<std::byte>& response) const;
};

} // namespace DnsPro
