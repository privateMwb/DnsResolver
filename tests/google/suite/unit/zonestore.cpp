// ZoneStore unit test suite.
//
// Coverage:
// - Names are matched case-insensitively (canonicalize() lowercases
//   before storing/looking up)
// - lookup() and contains() only match the requested record type, not
//   every record stored under a name
// - removeRecord() hit (existing name+type) and miss (wrong type,
//   wrong name) cases both return the correct bool
// - Removing the last record under a name cleans up the name's entry
//   entirely, so hasName()/contains() reflect that it's gone

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kTypeA = 1;     // A
constexpr std::uint16_t kTypeAAAA = 28; // AAAA
constexpr std::uint16_t kTypeMX = 15;   // MX, deliberately never stored
constexpr std::uint16_t kClass = 1;     // IN

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::ResourceRecord makeRecord(const Packet::Name& name, std::uint16_t type,
                                  std::uint8_t lastOctet) {
    Packet::ResourceRecord record;
    record.name = name;
    record.type = type;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{lastOctet});
    return record;
}

} // namespace

// Verifies a record stored under a mixed-case name is found by lookups
// using any other casing of the same name.
TEST(ZoneStore, CaseInsensitiveNameMatching) {
    ZoneStore store;
    store.addRecord(makeRecord(makeName("Example", "COM"), kTypeA, 1));

    EXPECT_TRUE(store.hasName(makeName("example", "com")));
    EXPECT_TRUE(store.hasName(makeName("EXAMPLE", "com")));
    EXPECT_TRUE(store.hasName(makeName("eXaMpLe", "CoM")));

    Vector<Packet::ResourceRecord> matches = store.lookup(makeName("example", "com"), kTypeA);
    EXPECT_EQ(matches.size(), 1);
}

// Verifies lookup() and contains() only match the requested type, even
// when the same name holds records of other types too.
TEST(ZoneStore, TypeFilteringLookupContains) {
    ZoneStore store;
    Packet::Name name = makeName("example", "com");
    store.addRecord(makeRecord(name, kTypeA, 1));
    store.addRecord(makeRecord(name, kTypeAAAA, 2));

    EXPECT_TRUE(store.contains(name, kTypeA));
    EXPECT_TRUE(store.contains(name, kTypeAAAA));
    EXPECT_FALSE(store.contains(name, kTypeMX));

    Vector<Packet::ResourceRecord> aMatches = store.lookup(name, kTypeA);
    ASSERT_EQ(aMatches.size(), 1);
    EXPECT_EQ(aMatches[0].type, kTypeA);

    Vector<Packet::ResourceRecord> mxMatches = store.lookup(name, kTypeMX);
    EXPECT_EQ(mxMatches.size(), 0);
}

// Verifies removeRecord() returns true on an existing name+type and
// false on both a wrong type and a name that was never stored.
TEST(ZoneStore, RemoveRecordHitAndMiss) {
    ZoneStore store;
    Packet::Name name = makeName("example", "com");
    store.addRecord(makeRecord(name, kTypeA, 1));

    EXPECT_TRUE(store.removeRecord(name, kTypeA));
    EXPECT_FALSE(store.removeRecord(name, kTypeA));                       // already removed
    EXPECT_FALSE(store.removeRecord(name, kTypeMX));                      // never existed
    EXPECT_FALSE(store.removeRecord(makeName("missing", "com"), kTypeA)); // name never existed
}

// Verifies removing the last record under a name cleans up the name's
// entry entirely, not just the record itself.
TEST(ZoneStore, RemoveRecordBucketCleanup) {
    ZoneStore store;
    Packet::Name name = makeName("example", "com");
    store.addRecord(makeRecord(name, kTypeA, 1));

    EXPECT_TRUE(store.hasName(name));
    EXPECT_TRUE(store.removeRecord(name, kTypeA));

    EXPECT_FALSE(store.hasName(name));
    EXPECT_FALSE(store.contains(name, kTypeA));
    EXPECT_EQ(store.recordCount(), 0);
}
