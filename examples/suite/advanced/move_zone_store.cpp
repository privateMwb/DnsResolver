// Move Zone Store.
//
// Demonstrates:
// - ZoneStore's move constructor and move-assignment operator transfer
//   ownership of the stored records rather than copying them
// - each moved-to store gets its own independent mutex, same as a copy
// - a moved-from store is left valid but should not be relied on for its
//   prior contents -- treat it the same as any other moved-from object

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
    setTitle("Populate a Store to Move From");

    ZoneStore source;
    source.addRecord(makeA("www", "example"));
    source.addRecord(makeA("mail", "example"));
    std::cout << "source.recordCount() before move : " << source.recordCount() << "\n\n";

    setTitle("Move-Construct Into a New Store");

    ZoneStore moved(std::move(source));
    std::cout << "moved.recordCount()  : " << moved.recordCount() << "\n\n";

    setTitle("Move-Assign Into an Existing Store");

    ZoneStore target;
    target.addRecord(makeA("ftp", "example")); // will be replaced

    target = std::move(moved);
    std::cout << "target.recordCount() : " << target.recordCount() << "\n";
}

REGISTER_EXAMPLE_SUITE();
