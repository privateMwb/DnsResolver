// Resolver concurrent resolve test suite.
//
// Coverage:
// - Concurrent resolve() calls against a shared, already-populated
//   ZoneStore each produce an independently correct answer
//
// Unlike zonestore_concurrent_access.cpp, this only exercises
// concurrent *reads*: lookup() is const, and the store is fully
// populated before any thread starts, so no thread ever calls
// addRecord()/removeRecord() concurrently with anything else here.
// That's safer in principle, but ZoneStore still documents no
// thread-safety guarantee either way - this validates an assumption
// about read-only HashMap access, not a guaranteed contract.

#include <support/framework.h>

#include <thread>
#include <vector>

using namespace DnsPro;

namespace {

constexpr std::uint16_t kType = 1;  // A
constexpr std::uint16_t kClass = 1; // IN

constexpr int kThreadCount = 8;
constexpr int kQueriesPerThread = 100;

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Vector<std::byte> buildQuery() {
    Packet::Question question;
    question.qname = makeName("example", "com");
    question.qtype = kType;
    question.qclass = kClass;

    Packet::Message message;
    message.questions.push_back(question);

    Vector<std::byte> bytes;
    Status s = Builder::build(message, bytes);
    (void)s;
    return bytes;
}

} // namespace

// Verifies concurrent resolve() calls against a shared store each
// produce a correct, independent answer.
static void concurrent_resolve_reads() {
    ZoneStore store;
    Packet::ResourceRecord record;
    record.name = makeName("example", "com");
    record.type = kType;
    record.rclass = kClass;
    record.ttl = 3600;
    record.rdata.push_back(std::byte{93});
    record.rdata.push_back(std::byte{184});
    record.rdata.push_back(std::byte{216});
    record.rdata.push_back(std::byte{34});
    store.addRecord(record); // store fully populated before any thread starts

    Resolver resolver(store);
    Vector<std::byte> query = buildQuery();
    std::span<const std::byte> queryBuffer(query.begin(), query.size());

    // Each thread writes to its own disjoint index range - distinct
    // std::vector<char> elements are independent memory locations, so
    // this part is race-free regardless of ZoneStore's own guarantees
    // (unlike std::vector<bool>, which bit-packs and would NOT be safe
    // for concurrent writes to different indices).
    std::vector<char> results(static_cast<std::size_t>(kThreadCount * kQueriesPerThread), 0);

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&, t] {
            for (int i = 0; i < kQueriesPerThread; ++i) {
                Vector<std::byte> response;
                Status resolveStatus = resolver.resolve(queryBuffer, response);
                if (resolveStatus != Status::OK)
                    continue;

                std::span<const std::byte> responseBuffer(response.begin(), response.size());
                Packet::Message parsed;
                Status parseStatus = Parser::parse(responseBuffer, parsed);
                if (parseStatus != Status::OK)
                    continue;

                bool ok = (parsed.header.rcode == 0) && (parsed.answers.size() == 1);
                results[static_cast<std::size_t>(t * kQueriesPerThread + i)] = ok ? 1 : 0;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    for (char r : results) {
        CHK(r == 1);
    }
}

// Executes all resolver-concurrent-resolve test cases.
static void run_tests() {
    RUN(concurrent_resolve_reads);
}

REGISTER_TEST_SUITE();
