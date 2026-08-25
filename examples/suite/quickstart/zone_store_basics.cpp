// Zone Store Basics.
//
// Demonstrates:
// - constructing a ZoneStore and adding records with addRecord()
// - looking records up by name and type with lookup()
// - contains() vs hasName() -- "no records of this type" vs "no name at all"
// - removing records and reading recordCount()

#include <support/framework.h>

using namespace DnsPro;

static Name makeName(const std::string& first, const std::string& second) {
    Name n;
    n.labels.push_back(first);
    n.labels.push_back(second);
    return n;
}

static void run_examples() {
    setTitle("Add a Record");

    ZoneStore zone;

    ResourceRecord a;
    a.name = makeName("example", "com");
    a.type = 1;   // A
    a.rclass = 1; // IN
    a.ttl = 300;
    a.rdata.push_back(std::byte{192});
    a.rdata.push_back(std::byte{0});
    a.rdata.push_back(std::byte{2});
    a.rdata.push_back(std::byte{1});
    zone.addRecord(a);

    std::cout << "recordCount() : " << zone.recordCount() << "\n\n";

    setTitle("Look It Up");

    Vector<ResourceRecord> found = zone.lookup(a.name, 1);
    std::cout << "lookup(example.com, A) count : " << found.size() << "\n";
    std::cout << "contains(example.com, A)     : " << zone.contains(a.name, 1) << "\n";
    std::cout << "contains(example.com, MX)    : " << zone.contains(a.name, 15) << "\n";
    std::cout << "hasName(example.com)         : " << zone.hasName(a.name) << "\n";

    Name missing = makeName("nowhere", "invalid");
    std::cout << "hasName(nowhere.invalid)     : " << zone.hasName(missing) << "\n\n";

    setTitle("Remove It");

    bool removed = zone.removeRecord(a.name, 1);
    std::cout << "removeRecord(example.com, A) : " << removed << "\n";
    std::cout << "recordCount() after remove   : " << zone.recordCount() << "\n";
}

REGISTER_EXAMPLE_SUITE();
