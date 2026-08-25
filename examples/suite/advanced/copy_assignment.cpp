// Copy Assignment.
//
// Demonstrates:
// - ZoneStore's copy constructor and copy-assignment operator produce a
//   fully independent store, not a shared/aliased one
// - each copy owns its own mutex, so the two stores can be used from
//   separate threads without contending on the same lock
// - mutating the copy afterward leaves the original untouched

#include <support/framework.h>

using namespace DnsPro;

static ResourceRecord makeA(const std::string& host, const std::string& domain) {
    ResourceRecord rr;
    rr.name.labels.push_back(host);
    rr.name.labels.push_back(domain);
    rr.type = 1;
    rr.rclass = 1;
    rr.ttl = 300;
    rr.rdata.push_back(std::byte{192});
    rr.rdata.push_back(std::byte{0});
    rr.rdata.push_back(std::byte{2});
    rr.rdata.push_back(std::byte{1});
    return rr;
}

static void run_examples() {
    setTitle("Populate the Original");

    ZoneStore original;
    original.addRecord(makeA("www", "example"));
    std::cout << "original.recordCount() : " << original.recordCount() << "\n\n";

    setTitle("Copy-Construct, Then Diverge");

    ZoneStore copy(original);
    copy.addRecord(makeA("mail", "example"));

    std::cout << "copy.recordCount()     : " << copy.recordCount() << "\n";
    std::cout << "original.recordCount() : " << original.recordCount() << "  (unchanged)\n\n";

    setTitle("Copy-Assign Over a Third Store");

    ZoneStore third;
    third.addRecord(makeA("ftp", "example"));
    third = original; // replaces third's contents with a copy of original's

    std::cout << "third.recordCount()    : " << third.recordCount() << "\n";
}

REGISTER_EXAMPLE_SUITE();
