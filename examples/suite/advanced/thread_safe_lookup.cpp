// Thread Safe Lookup.
//
// Demonstrates:
// - ZoneStore's readers (lookup, contains, hasName, recordCount) may run
//   concurrently from multiple threads -- they take a shared lock
// - a writer (addRecord) takes an exclusive lock, so it's safe to
//   populate a store before handing out concurrent readers
// - no external synchronization is needed around the ZoneStore itself

#include <support/framework.h>

#include <atomic>
#include <thread>
#include <vector>

using namespace DnsPro;

static void run_examples() {
    setTitle("Populate the Zone Once, Up Front");

    ZoneStore zone;
    for (int i = 0; i < 10; ++i) {
        ResourceRecord rr;
        rr.name.labels.push_back("host" + std::to_string(i));
        rr.name.labels.push_back("example");
        rr.type = 1;
        rr.rclass = 1;
        rr.ttl = 300;
        rr.rdata.push_back(std::byte{192});
        rr.rdata.push_back(std::byte{0});
        rr.rdata.push_back(std::byte{2});
        rr.rdata.push_back(std::byte{static_cast<std::uint8_t>(i)});
        zone.addRecord(rr);
    }
    std::cout << "recordCount() before reads : " << zone.recordCount() << "\n\n";

    setTitle("Read Concurrently From Several Threads");

    std::vector<std::thread> readers;
    std::atomic<int> totalFound{0};

    for (int t = 0; t < 4; ++t) {
        readers.emplace_back([&zone, &totalFound, t]() {
            for (int i = t; i < 10; i += 4) {
                Name name;
                name.labels.push_back("host" + std::to_string(i));
                name.labels.push_back("example");
                if (zone.contains(name, 1)) {
                    totalFound.fetch_add(1);
                }
            }
        });
    }

    for (auto& reader : readers) {
        reader.join();
    }

    std::cout << "records found across all readers : " << totalFound.load() << "\n";
}

REGISTER_EXAMPLE_SUITE();
