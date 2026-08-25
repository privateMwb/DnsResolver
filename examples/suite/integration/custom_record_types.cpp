// Custom Record Types.
//
// Demonstrates:
// - qtype/type fields are plain std::uint16_t everywhere in this library
//   -- Parser, Builder, and ZoneStore treat record types as opaque
//   numbers, not a fixed enum, so nothing needs updating to support a
//   type the library's author never heard of
// - defining application-side named constants (including RFC-registered
//   types the library doesn't hardcode, and private-use types)
// - storing and looking up a private-use type the same way as A or MX

#include <support/framework.h>

using namespace DnsPro;

// Named constants an application defines for its own clarity -- DnsPro
// itself never references these names, only the numeric values.
namespace RecordType {
constexpr std::uint16_t TXT = 16;
constexpr std::uint16_t PRIVATE_USE = 65280; // first of the 65280-65534 private-use range
} // namespace RecordType

static void run_examples() {
    setTitle("Store a TXT Record");

    ZoneStore zone;
    Name domain;
    domain.labels.push_back("example");
    domain.labels.push_back("com");

    ResourceRecord txt;
    txt.name = domain;
    txt.type = RecordType::TXT;
    txt.rclass = 1;
    txt.ttl = 300;
    std::string text = "v=spf1 -all";
    txt.rdata.push_back(std::byte{static_cast<std::uint8_t>(text.size())});
    for (char c : text) {
        txt.rdata.push_back(std::byte{static_cast<std::uint8_t>(c)});
    }
    zone.addRecord(txt);

    std::cout << "contains(example.com, TXT) : " << zone.contains(domain, RecordType::TXT)
              << "\n\n";

    setTitle("Store a Private-Use Record Type");

    ResourceRecord custom;
    custom.name = domain;
    custom.type = RecordType::PRIVATE_USE;
    custom.rclass = 1;
    custom.ttl = 60;
    custom.rdata.push_back(std::byte{0x01});
    custom.rdata.push_back(std::byte{0x02});
    zone.addRecord(custom);

    Vector<ResourceRecord> found = zone.lookup(domain, RecordType::PRIVATE_USE);
    std::cout << "lookup(example.com, 65280) size : " << found.size() << "\n";
    std::cout << "recordCount()                   : " << zone.recordCount() << "\n";
}

REGISTER_EXAMPLE_SUITE();
