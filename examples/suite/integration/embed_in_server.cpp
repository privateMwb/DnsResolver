// Embed in Server.
//
// Demonstrates:
// - wrapping ZoneStore + Resolver inside a small owning class, the shape
//   a real application embeds the library in (Resolver only ever
//   references a ZoneStore, so something has to own the ZoneStore)
// - exposing a simple handleQuery(bytes) -> bytes surface to callers,
//   hiding Parser/Builder/Status details behind the wrapper
// - loading zone data once at construction, then serving many queries

#include <support/framework.h>

using namespace DnsPro;

class DnsServer {
  public:
    DnsServer() : resolver_(zone_) {}

    void loadRecord(const ResourceRecord& record) {
        zone_.addRecord(record);
    }

    // Returns the raw response bytes, or an empty buffer if the query
    // itself couldn't be parsed at all.
    Vector<std::byte> handleQuery(std::span<const std::byte> query) {
        Vector<std::byte> response;
        Status status = resolver_.resolve(query, response);
        if (status != Status::OK) {
            return Vector<std::byte>{};
        }
        return response;
    }

  private:
    ZoneStore zone_;
    Resolver resolver_;
};

static void run_examples() {
    setTitle("Construct and Load the Server");

    DnsServer server;

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
    server.loadRecord(a);

    setTitle("Handle a Query Through the Wrapper");

    Message queryMessage;
    Question q;
    q.qname = a.name;
    q.qtype = 1;
    q.qclass = 1;
    queryMessage.questions.push_back(q);

    Vector<std::byte> query;
    (void)Builder::build(queryMessage, query);

    Vector<std::byte> response =
        server.handleQuery(std::span<const std::byte>(query.begin(), query.size()));
    std::cout << "response size : " << response.size() << " bytes\n";

    Message parsed;
    (void)Parser::parse(std::span<const std::byte>(response.begin(), response.size()), parsed);
    std::cout << "rcode         : " << static_cast<int>(parsed.header.rcode) << "\n";
    std::cout << "answers       : " << parsed.answers.size() << "\n";
}

REGISTER_EXAMPLE_SUITE();
