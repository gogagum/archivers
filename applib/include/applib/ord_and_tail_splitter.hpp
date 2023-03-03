#ifndef APPLIB_ORD_AND_TAIL_SPLITTER
#define APPLIB_ORD_AND_TAIL_SPLITTER

#include <algorithm>
#include <vector>
#include <cstdint>
#include <span>
#include <boost/container/static_vector.hpp>
#include <applib/words_and_flow.hpp>

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAAdaptiveEncodeImpl class
///
class OrdAndTailSplitter {
public:
    struct Ret {
        std::vector<std::uint64_t> ords;
        boost::container::static_vector<bool, 32> tail;
    };

public:
    static Ret process(const std::span<const std::byte>& inData,
                       std::uint8_t numBits);

private:
    template <std::uint8_t bitsNum>
    static Ret _process(const std::span<const std::byte>& inData);
};

////////////////////////////////////////////////////////////////////////////////
template <std::uint8_t bitsNum>
auto OrdAndTailSplitter::_process(
        const std::span<const std::byte>& inData) -> Ret {
    auto flow = Flow<bitsNum>(inData);
    std::vector<std::uint64_t> retOrds;
    auto outIter = std::back_inserter(retOrds);
    std::transform(flow.begin(), flow.end(), outIter,
                   [](const Word<bitsNum>& w) {
                       return Word<bitsNum>::ord(w);
                   });
    auto flowTail = flow.getTail();
    auto retTail = boost::container::static_vector<bool, 32>(
                flowTail.begin(), flowTail.end());
    return { retOrds, retTail };
}

#endif  // APPLIB_ORD_AND_TAIL_SPLITTER
