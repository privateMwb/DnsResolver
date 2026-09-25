// ZoneStore concurrent access test suite.
//
// Coverage:
// - Concurrent addRecord() calls under distinct names all land, with
//   the correct final recordCount()
//
// ZoneStore has no internal synchronization - just a plain HashMap, no
// mutex. This test exercises real data races on that HashMap and is
// expected to be flagged by ThreadSanitizer (or crash outright) until
// ZoneStore gains a lock. It's written as documentation of that gap,
// not a currently-guaranteed pass.

#include <DnsPro/DnsResolver.h>

#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <vector>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

constexpr int kThreadCount = 8;
constexpr int kRecordsPerThread = 50;

Packet::Name makeName(const std::string& host) {
    Packet::Name name;
    name.labels.push_back(host);
    name.labels.push_back("example");
    return name;
}

Packet::ResourceRecord makeRecord(const std::string& host) {
    Packet::ResourceRecord record;
    record.name = makeName(host);
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{1});
    return record;
}

} // namespace

// Verifies concurrent addRecord() calls under distinct names all land.
TEST(ZoneStoreConcurrentAccess, ConcurrentAddRecord) {
    ZoneStore store;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&store, t] {
            for (int i = 0; i < kRecordsPerThread; ++i) {
                std::string host = "t" + std::to_string(t) + "r" + std::to_string(i);
                store.addRecord(makeRecord(host));
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(store.recordCount(), static_cast<std::size_t>(kThreadCount * kRecordsPerThread));
}
