// ZoneStore copy lifecycle test suite.
//
// Coverage:
// - Copying a ZoneStore produces a fully independent store: mutating
//   the copy doesn't affect the original, and vice versa

#include <support/framework.h>

using namespace DnsPro;

namespace {

Packet::Name makeName(const std::string& first, const std::string& second) {
    Packet::Name name;
    name.labels.push_back(first);
    name.labels.push_back(second);
    return name;
}

Packet::ResourceRecord makeRecord(const std::string& first, const std::string& second) {
    Packet::ResourceRecord record;
    record.name = makeName(first, second);
    record.type = 1;   // A
    record.rclass = 1; // IN
    record.ttl = 3600;
    record.rdata.push_back(std::byte{1});
    return record;
}

} // namespace

// Verifies a copied ZoneStore is independent of its source.
static void copy_is_independent() {
    ZoneStore original;
    original.addRecord(makeRecord("example", "com"));
    CHK(original.recordCount() == 1);

    // ZoneStore contains a shared_mutex, so copying requires the
    // explicitly defined copy constructor.
    ZoneStore copy(original);

    // Mutating the copy shouldn't touch the original.
    copy.addRecord(makeRecord("other", "com"));
    CHK(copy.recordCount() == 2);
    CHK(original.recordCount() == 1);

    // Mutating the original after the copy was made shouldn't touch
    // the copy either.
    original.addRecord(makeRecord("third", "com"));
    CHK(original.recordCount() == 2);
    CHK(copy.recordCount() == 2);

    CHK(original.hasName(makeName("third", "com")));
    CHK(!copy.hasName(makeName("third", "com")));

    CHK(copy.hasName(makeName("other", "com")));
    CHK(!original.hasName(makeName("other", "com")));
}

// Executes all ZoneStore-copy test cases.
static void run_tests() {
    RUN(copy_is_independent);
}

REGISTER_TEST_SUITE();