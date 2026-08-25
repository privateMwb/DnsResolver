// Multi Type Records.
//
// Demonstrates:
// - one name holding records of several types (A and MX) at once
// - lookup(name, type) only ever returning that one type's records
// - hasName() staying true as long as any type remains, even after a
//   specific type's records are all removed

#include <support/framework.h>

using namespace DnsPro;

static void run_examples() {
    setTitle("Add an A Record and an MX Record for the Same Name");

    ZoneStore zone;

    Name domain;
    domain.labels.push_back("example");
    domain.labels.push_back("com");

    ResourceRecord a;
    a.name = domain;
    a.type = 1; // A
    a.rclass = 1;
    a.ttl = 300;
    a.rdata.push_back(std::byte{192});
    a.rdata.push_back(std::byte{0});
    a.rdata.push_back(std::byte{2});
    a.rdata.push_back(std::byte{1});
    zone.addRecord(a);

    ResourceRecord mx;
    mx.name = domain;
    mx.type = 15; // MX
    mx.rclass = 1;
    mx.ttl = 300;
    mx.rdata.push_back(std::byte{0});  // preference, high byte
    mx.rdata.push_back(std::byte{10}); // preference, low byte
    zone.addRecord(mx);

    std::cout << "recordCount()            : " << zone.recordCount() << "\n\n";

    setTitle("Look Up Each Type Independently");

    std::cout << "lookup(example.com, A)  size : " << zone.lookup(domain, 1).size() << "\n";
    std::cout << "lookup(example.com, MX) size : " << zone.lookup(domain, 15).size() << "\n";
    std::cout << "lookup(example.com, AAAA) size: " << zone.lookup(domain, 28).size() << "\n\n";

    setTitle("Remove One Type, the Name Survives");

    (void)zone.removeRecord(domain, 1); // drop the A record only
    std::cout << "hasName(example.com)         : " << zone.hasName(domain) << "\n";
    std::cout << "contains(example.com, A)     : " << zone.contains(domain, 1) << "\n";
    std::cout << "contains(example.com, MX)    : " << zone.contains(domain, 15) << "\n";
}

REGISTER_EXAMPLE_SUITE();
