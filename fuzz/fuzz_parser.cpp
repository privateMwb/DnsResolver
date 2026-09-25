// ============================================================
// fuzz/fuzz_parser.cpp
//
// Round-trip fuzzer for Parser::parse() / Builder::build(). There's no
// reference DNS implementation to differentially compare against (see
// FUZZING.md), so this instead checks the one correctness property the
// library actually promises for these two: a rebuilt packet re-parses
// to an equivalent Message (README, "Builder" highlight; Builder.h's
// own doc comment says the same thing).
//
// Concretely, for input that parses successfully:
//   buffer --parse--> message --build--> rebuilt
//   rebuilt --parse--> reparsed --build--> rebuiltAgain
//
// `rebuilt` is not expected to equal the original `buffer` byte-for-byte
// -- Builder never emits compression pointers (see Builder.h), so a
// compressed input can rebuild to a larger, uncompressed encoding. What
// *is* expected is a fixed point one hop later: rebuiltAgain must equal
// rebuilt exactly. If it doesn't, Parser and Builder disagree with each
// other about what some Message means, which is exactly the class of
// bug this harness exists to catch.
//
// Also exercises, incidentally, every failure path a malformed packet
// can take before reaching that point: truncated buffers
// (Status::BUFFER_TOO_SMALL), invalid label encodings
// (Status::MALFORMED_NAME), and -- the trickiest part of RFC 1035 to
// get right -- compression-pointer loops and forward pointers
// (Status::COMPRESSION_LOOP, Status::COMPRESSION_FORWARD_POINTER).
// Parser bounds total pointer hops with a step counter specifically to
// terminate on the oscillating case (forward label, then a pointer back
// to it) that a naive "each hop must be backward" check would miss --
// under a fuzzer, that's a hang/timeout, not a clean rejection, if it's
// ever wrong.
// ============================================================

#include <DnsPro/Builder.h>
#include <DnsPro/Packet/Message.h>
#include <DnsPro/Parser.h>
#include <DnsPro/Status.h>

#include <VectorPro/Vector.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>

using namespace DnsPro;
using VectorPro::Vector;

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    const auto buffer = std::span<const std::byte>(reinterpret_cast<const std::byte*>(data), size);

    Packet::Message message;
    if (Parser::parse(buffer, message) != Status::OK)
        return 0;

    // A message that just parsed successfully came from a buffer with
    // already-bounded label lengths (<=63 bytes), already-bounded rdata
    // (rdlength is a 16-bit field), and section counts that matched the
    // header -- Builder should never fail to re-serialize it.
    Vector<std::byte> rebuilt;
    if (Builder::build(message, rebuilt) != Status::OK)
        std::abort();

    Packet::Message reparsed;
    if (Parser::parse(rebuilt.as_span(), reparsed) != Status::OK)
        std::abort();

    Vector<std::byte> rebuiltAgain;
    if (Builder::build(reparsed, rebuiltAgain) != Status::OK)
        std::abort();

    if (rebuilt != rebuiltAgain)
        std::abort();

    return 0;
}
