/**
 * @file Resolver.cpp
 * @brief DnsPro::Resolver implementation.
 *
 * Contains the implementation of Resolver's query-to-response pipeline.
 */

// ============================================================
// Implementation for DnsPro::Resolver.
// ============================================================
//
//  Sections:
//   1. Constructor
//   2. Resolution
//
// ============================================================

#include <DnsPro/Resolver.h> // Resolver

#include <utility> // std::move

namespace DnsPro {

// ============================================================
//  Section 1 — Constructor
// ============================================================
Resolver::Resolver(ZoneStore& zone) noexcept : zone_(zone) {}


// ============================================================
//  Section 2 — Resolution
// ============================================================
Status Resolver::resolve(std::span<const std::byte> query, Vector<std::byte>& response) const {
    Packet::Message queryMessage;
    if (Status s = Parser::parse(query, queryMessage); s != Status::OK) return s;

    Packet::Message responseMessage;
    responseMessage.header       = queryMessage.header;
    responseMessage.header.qr    = 1; // this is a response
    responseMessage.header.aa    = 1; // authoritative -- answers come straight from zone_
    responseMessage.header.ra    = 0; // no recursive lookup path exists
    responseMessage.header.rcode = RCODE_NOERROR;

    if (queryMessage.questions.empty()) {
        // Nothing to answer. Not a parse failure (the buffer was
        // well-formed), but a query with no question is malformed
        // according to what a resolver needs to do anything useful with it.
        responseMessage.header.rcode = RCODE_FORMERR;
        return Builder::build(responseMessage, response);
    }

    // Only the first question is answered -- see Resolver::resolve()'s doc comment.
    const Packet::Question& question = queryMessage.questions[0];
    responseMessage.questions.push_back(question);

    Vector<Packet::ResourceRecord> matches = zone_.lookup(question.qname, question.qtype);

    if (!matches.empty()) {
        responseMessage.answers = std::move(matches);
    } else if (!zone_.hasName(question.qname)) {
        // No records of this type, and the name itself doesn't exist either.
        responseMessage.header.rcode = RCODE_NXDOMAIN;
    }
    // else: name exists, just not with this record type -- NOERROR with an
    // empty answer section (NODATA). responseMessage.header.rcode is
    // already RCODE_NOERROR from above; nothing further to set.

    return Builder::build(responseMessage, response);
}

} // namespace DnsPro
