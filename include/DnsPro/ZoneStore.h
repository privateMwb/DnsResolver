/**
 * @file            ZoneStore.h
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
#include <DnsPro/Packet/Name.h>           // Name
#include <DnsPro/Packet/ResourceRecord.h> // ResourceRecord

#include <HashMapPro/HashMap.h> // HashMapPro::HashMap
#include <VectorPro/Vector.h>   // VectorPro::Vector

#include <cstddef>       // std::size_t
#include <cstdint>       // std::uint16_t
#include <shared_mutex>  // std::shared_mutex
#include <string>        // std::string
// clang-format on

// In-memory name -> record(s) store, backed by HashMapPro, nested two
// levels deep: name -> record type -> records of that type. The inner
// type-indexed map means lookup(name, type) and removeRecord(name, type)
// are two HashMap lookups, not a linear scan over every record a name
// has regardless of type. Keys are canonicalized domain names (labels
// joined with '.', lowercased -- DNS names are case-insensitive per
// RFC 1035 S2.3.3 / RFC 4343), so "Example.COM" and "example.com" resolve
// to the same entry.
//
// All public operations are synchronized by mutex_. Multiple readers may
// access the store concurrently, while operations that modify the store
// acquire exclusive ownership.

namespace DnsPro {

using namespace HashMapPro;
using namespace VectorPro;

/**
 * @brief An in-memory store mapping domain names to their resource
 * records, indexed by both name and record type.
 *
 * @details Holds no raw owning pointers -- HashMap and Vector both own
 * their storage directly. All public operations are thread-safe.
 */
class ZoneStore {
  private:
    using TypeMap = HashMap<std::uint16_t, Vector<Packet::ResourceRecord>>;

    HashMap<std::string, TypeMap> records_; ///< Canonical name -> its records, indexed by type.

    std::size_t recordCount_ = 0; ///< Total records across all names and types, maintained
                                  ///< incrementally by addRecord()/removeRecord()/clear().

    mutable std::shared_mutex mutex_;

  public:
    /// @brief Constructs an empty ZoneStore.
    ZoneStore() = default;

    /**
     * @brief Copy-constructs a ZoneStore.
     * @details Copies the stored records and count while creating a new,
     * independent mutex for the new instance.
     */
    ZoneStore(const ZoneStore& other);

    /**
     * @brief Copy-assigns a ZoneStore.
     * @details Copies the stored records and count while keeping this
     * instance's own mutex.
     */
    ZoneStore& operator=(const ZoneStore& other);

    /**
     * @brief Move-constructs a ZoneStore.
     * @details Moves the stored records and count while creating a new,
     * independent mutex for the new instance.
     */
    ZoneStore(ZoneStore&& other) noexcept;

    /**
     * @brief Move-assigns a ZoneStore.
     * @details Moves the stored records and count while keeping this
     * instance's own mutex.
     */
    ZoneStore& operator=(ZoneStore&& other) noexcept;

    /**
     * @brief Adds a record to the store.
     * @param record Record to add. Its own `name` and `type` determine
     * which name+type bucket it's appended to.
     * @details Always appends -- multiple records sharing a name and type
     * (round-robin) are valid and expected. Does not deduplicate or
     * replace existing records.
     */
    void addRecord(const Packet::ResourceRecord& record);

    /**
     * @brief Removes every record matching `name` and `type`.
     * @param name Name to match.
     * @param type Record type to match.
     * @return `true` if at least one record was removed, `false` if none
     * matched (whether because `name` has no entry at all, or it does but
     * holds no records of `type`).
     */
    [[nodiscard]] bool removeRecord(const Packet::Name& name, std::uint16_t type);

    /**
     * @brief Looks up records by name and type.
     * @param name Name to search for.
     * @param type Record type to match.
     * @return A copy of the matching records, or an empty Vector if
     * `name` has no entry or holds no records of `type`.
     */
    [[nodiscard]] Vector<Packet::ResourceRecord> lookup(const Packet::Name& name,
                                                        std::uint16_t type) const;

    /// @brief Checks whether `name` has any records of `type` stored.
    [[nodiscard]] bool contains(const Packet::Name& name, std::uint16_t type) const noexcept;

    /**
     * @brief Checks whether `name` has any records stored, of any type.
     * @details Distinct from contains(name, type): this is what lets a
     * caller (Resolver) tell "name doesn't exist" (NXDOMAIN) apart from
     * "name exists, just not with this record type" (NOERROR/NODATA).
     */
    [[nodiscard]] bool hasName(const Packet::Name& name) const noexcept;

    /// @brief Removes every record for every name. Bucket storage capacity is unaffected.
    void clear() noexcept;

    /// @brief Returns the total number of records stored, across all names and types.
    [[nodiscard]] std::size_t recordCount() const noexcept;

  private:
    /**
     * @brief Canonicalizes a Name into its HashMap key.
     * @param name Name to canonicalize.
     * @return `name`'s labels joined with `.` and lowercased (ASCII only
     * -- DNS labels are ASCII per RFC 1035; there is no IDNA/punycode
     * handling here).
     */
    [[nodiscard]] static std::string canonicalize(const Packet::Name& name);
};

} // namespace DnsPro