/**
 * @file ZoneStore.cpp
 * @brief DnsPro::ZoneStore implementation.
 *
 * Contains the implementation of ZoneStore's record management,
 * copy/move lifecycle, synchronization, and name canonicalization.
 */

// ============================================================
// Implementation for DnsPro::ZoneStore.
// ============================================================
//
//  Sections:
//   1. Special Member Functions
//   2. Record Management
//   3. Introspection
//   4. Helper
//
// ============================================================

// clang-format off
#include <DnsPro/ZoneStore.h> // ZoneStore

#include <cctype>       // std::tolower
#include <mutex>        // std::scoped_lock
#include <shared_mutex> // std::shared_lock, std::unique_lock
#include <utility>      // std::move
// clang-format on

namespace DnsPro {

// ============================================================
//  Section 1 — Special Member Functions
// ============================================================

ZoneStore::ZoneStore(const ZoneStore& other) {
    std::shared_lock lock(other.mutex_);

    records_ = other.records_;
    recordCount_ = other.recordCount_;
}

ZoneStore& ZoneStore::operator=(const ZoneStore& other) {
    if (this == &other)
        return *this;

    std::scoped_lock lock(mutex_, other.mutex_);

    records_ = other.records_;
    recordCount_ = other.recordCount_;

    return *this;
}

ZoneStore::ZoneStore(ZoneStore&& other) noexcept {
    std::unique_lock lock(other.mutex_);

    records_ = std::move(other.records_);
    recordCount_ = other.recordCount_;

    other.recordCount_ = 0;
}

ZoneStore& ZoneStore::operator=(ZoneStore&& other) noexcept {
    if (this == &other)
        return *this;

    std::scoped_lock lock(mutex_, other.mutex_);

    records_ = std::move(other.records_);
    recordCount_ = other.recordCount_;

    other.recordCount_ = 0;

    return *this;
}

// ============================================================
//  Section 2 — Record Management
// ============================================================

void ZoneStore::addRecord(const Packet::ResourceRecord& record) {
    std::unique_lock lock(mutex_);

    std::string key = canonicalize(record.name);

    records_[key][record.type].push_back(record);
    ++recordCount_;
}

bool ZoneStore::removeRecord(const Packet::Name& name,
                             std::uint16_t type) {
    std::unique_lock lock(mutex_);

    std::string key = canonicalize(name);

    if (!records_.contains(key))
        return false;

    TypeMap& typeMap = records_.at(key);

    if (!typeMap.contains(type))
        return false;

    recordCount_ -= typeMap.at(type).size();

    (void)typeMap.erase(type);

    // Clean up the now-empty outer entry so contains()/lookup()
    // and future adds don't have to look through a name with no
    // record types left.
    if (typeMap.empty()) {
        (void)records_.erase(key);
    }

    return true;
}

// ============================================================
//  Section 3 — Introspection
// ============================================================

Vector<Packet::ResourceRecord> ZoneStore::lookup(
    const Packet::Name& name,
    std::uint16_t type) const {

    std::shared_lock lock(mutex_);

    std::string key = canonicalize(name);

    if (!records_.contains(key))
        return Vector<Packet::ResourceRecord>{};

    const TypeMap& typeMap = records_.at(key);

    if (!typeMap.contains(type))
        return Vector<Packet::ResourceRecord>{};

    return typeMap.at(type);
}

bool ZoneStore::contains(const Packet::Name& name,
                         std::uint16_t type) const noexcept {

    std::shared_lock lock(mutex_);

    std::string key = canonicalize(name);

    if (!records_.contains(key))
        return false;

    return records_.at(key).contains(type);
}

bool ZoneStore::hasName(const Packet::Name& name) const noexcept {
    std::shared_lock lock(mutex_);

    return records_.contains(canonicalize(name));
}

void ZoneStore::clear() noexcept {
    std::unique_lock lock(mutex_);

    records_.clear();
    recordCount_ = 0;
}

std::size_t ZoneStore::recordCount() const noexcept {
    std::shared_lock lock(mutex_);

    return recordCount_;
}

// ============================================================
//  Section 4 — Helper
// ============================================================

std::string ZoneStore::canonicalize(const Packet::Name& name) {
    std::string key;

    for (std::size_t i = 0; i < name.labels.size(); ++i) {
        if (i > 0)
            key += '.';

        for (char c : name.labels[i]) {
            key += static_cast<char>(
                std::tolower(static_cast<unsigned char>(c))
            );
        }
    }

    return key;
}

} // namespace DnsPro