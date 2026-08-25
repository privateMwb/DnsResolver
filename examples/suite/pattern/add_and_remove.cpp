// Add and Remove.
//
// Demonstrates:
// - the standard add/verify/remove/verify cycle for ZoneStore records
// - checking removeRecord()'s bool return instead of assuming success
// - the difference between removing "the last record of a type" (bucket
//   is pruned) and removing "one of several"

#include <support/framework.h>

using namespace DnsPro;

static ResourceRecord makeA(const std::string& host, const std::string& domain,
                            std::uint8_t lastOctet) {
    ResourceRecord rr;
    rr.name.labels.push_back(host);
    rr.name.labels.push_back(domain);
    rr.type = 1;   // A
    rr.rclass = 1; // IN
    rr.ttl = 300;
    rr.rdata.push_back(std::byte{192});
    rr.rdata.push_back(std::byte{0});
    rr.rdata.push_back(std::byte{2});
    rr.rdata.push_back(std::byte{lastOctet});
    return rr;
}

static void run_examples() {
    setTitle("Add Two Records, Same Name and Type");

    ZoneStore zone;
    ResourceRecord first = makeA("www", "example", 1);
    ResourceRecord second = makeA("www", "example", 2);
    zone.addRecord(first);
    zone.addRecord(second);

    std::cout << "recordCount()              : " << zone.recordCount() << "\n";
    std::cout << "lookup(www.example, A) size: " << zone.lookup(first.name, 1).size() << "\n\n";

    setTitle("Remove One, the Other Stays");

    bool removedFirst = zone.removeRecord(first.name, 1);
    std::cout << "removeRecord() first call  : " << removedFirst << "\n";
    std::cout << "recordCount() after        : " << zone.recordCount() << "\n";
    std::cout << "hasName(www.example)       : " << zone.hasName(first.name) << "\n\n";

    setTitle("Remove the Last One, Name Disappears");

    bool removedSecond = zone.removeRecord(first.name, 1);
    std::cout << "removeRecord() second call : " << removedSecond << "\n";
    std::cout << "hasName(www.example)       : " << zone.hasName(first.name) << "\n";

    // Calling removeRecord() a third time finds nothing left to remove.
    bool removedThird = zone.removeRecord(first.name, 1);
    std::cout << "removeRecord() third call  : " << removedThird << "\n";
}

REGISTER_EXAMPLE_SUITE();
