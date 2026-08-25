// Authoritative Lookup.
//
// Demonstrates:
// - the standard shape of an authoritative answer: zone + Resolver, one
//   call in, one call out
// - the three outcomes a caller of resolve() distinguishes by rcode and
//   answer count -- an answer, NXDOMAIN, and NODATA

#include <support/framework.h>

using namespace DnsPro;

static Vector<std::byte> buildQuery(const std::string& host, const std::string& domain,
                                    std::uint16_t qtype) {
    Message m;
    Question q;
    q.qname.labels.push_back(host);
    q.qname.labels.push_back(domain);
    q.qtype = qtype;
    q.qclass = 1; // IN
    m.questions.push_back(q);

    Vector<std::byte> bytes;
    (void)Builder::build(m, bytes);
    return bytes;
}

static void askAndReport(const Resolver& resolver, const std::string& label,
                         const std::string& host, const std::string& domain, std::uint16_t qtype) {
    Vector<std::byte> query = buildQuery(host, domain, qtype);

    Vector<std::byte> response;
    (void)resolver.resolve(std::span<const std::byte>(query.begin(), query.size()), response);

    Message parsed;
    (void)Parser::parse(std::span<const std::byte>(response.begin(), response.size()), parsed);

    std::cout << label << " -> rcode " << static_cast<int>(parsed.header.rcode) << ", answers "
              << parsed.answers.size() << "\n";
}

static void run_examples() {
    setTitle("Set Up a Zone With One Name");

    ZoneStore zone;
    ResourceRecord a;
    a.name.labels.push_back("example");
    a.name.labels.push_back("com");
    a.type = 1;
    a.rclass = 1;
    a.ttl = 300;
    a.rdata.push_back(std::byte{192});
    a.rdata.push_back(std::byte{0});
    a.rdata.push_back(std::byte{2});
    a.rdata.push_back(std::byte{1});
    zone.addRecord(a);

    Resolver resolver(zone);

    setTitle("Three Kinds of Answer");

    askAndReport(resolver, "A for example.com  ", "example", "com", 1); // NOERROR, 1 answer
    askAndReport(resolver, "MX for example.com ", "example", "com",
                 15); // NOERROR, 0 answers (NODATA)
    askAndReport(resolver, "A for nowhere.com  ", "nowhere", "com", 1); // NXDOMAIN
}

REGISTER_EXAMPLE_SUITE();
