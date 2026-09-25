// ZoneStore copy lifecycle test suite.
//
// Coverage:
// - Copying a ZoneStore produces a fully independent store: mutating
//   the copy doesn't affect the original, and vice versa

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

using namespace DnsPro;

namespace {

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::ResourceRecord makeRecord(const std::string& first, const std::string& second) {
    Packet::ResourceRecord record;
    record.name = makeName(first, second);
    record.type = 1;   // A
    record.rclass = 1; // IN
    record.ttl = 3600;
    record.rdata.push_back(std::byte{1});
    return record;
}

} // namespace

// Verifies a copied ZoneStore is independent of its source.
TEST(ZoneStoreCopy, CopyIsIndependent) {
    ZoneStore original;
    original.addRecord(makeRecord("example", "com"));
    EXPECT_EQ(original.recordCount(), 1);

    ZoneStore copy = original; // copy constructor

    // Mutating the copy shouldn't touch the original.
    copy.addRecord(makeRecord("other", "com"));
    EXPECT_EQ(copy.recordCount(), 2);
    EXPECT_EQ(original.recordCount(), 1);

    // Mutating the original after the copy was made shouldn't touch the copy either.
    original.addRecord(makeRecord("third", "com"));
    EXPECT_EQ(original.recordCount(), 2);
    EXPECT_EQ(copy.recordCount(), 2);

    EXPECT_TRUE(original.hasName(makeName("third", "com")));
    EXPECT_FALSE(copy.hasName(makeName("third", "com")));

    EXPECT_TRUE(copy.hasName(makeName("other", "com")));
    EXPECT_FALSE(original.hasName(makeName("other", "com")));
}
