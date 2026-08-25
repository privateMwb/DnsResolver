// Std Algorithm Use.
//
// Demonstrates:
// - the Vector<T> returned by ZoneStore::lookup() exposes begin()/end(),
//   so it works directly with <algorithm> the same way a std::vector would
// - std::count_if and std::any_of over a lookup() result, no manual
//   index-based loop required

#include <support/framework.h>

#include <algorithm>

using namespace DnsPro;

static void run_examples() {
    setTitle("Populate Several A Records for One Name");

    ZoneStore zone;
    Name domain;
    domain.labels.push_back("pool");
    domain.labels.push_back("example");

    for (std::uint8_t i = 1; i <= 5; ++i) {
        ResourceRecord rr;
        rr.name = domain;
        rr.type = 1;
        rr.rclass = 1;
        rr.ttl = (i % 2 == 0) ? 60 : 300; // alternate short/long TTLs
        rr.rdata.push_back(std::byte{192});
        rr.rdata.push_back(std::byte{0});
        rr.rdata.push_back(std::byte{2});
        rr.rdata.push_back(std::byte{i});
        zone.addRecord(rr);
    }

    Vector<ResourceRecord> records = zone.lookup(domain, 1);
    std::cout << "records found : " << records.size() << "\n\n";

    setTitle("Query the Results With <algorithm>");

    long shortTtlCount = std::count_if(records.begin(), records.end(),
                                       [](const ResourceRecord& rr) { return rr.ttl < 300; });
    std::cout << "records with ttl < 300 : " << shortTtlCount << "\n";

    bool anyLongTtl = std::any_of(records.begin(), records.end(),
                                  [](const ResourceRecord& rr) { return rr.ttl >= 300; });
    std::cout << "any record with ttl >= 300 : " << anyLongTtl << "\n";
}

REGISTER_EXAMPLE_SUITE();
