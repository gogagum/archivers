#ifndef APPLIB_ORD_AND_TAIL_SPLITTER
#define APPLIB_ORD_AND_TAIL_SPLITTER

#include <algorithm>
#include <vector>
#include <cstdint>
#include <span>
#include <boost/container/static_vector.hpp>
#include <applib/exceptions.hpp>
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
                       std::uint8_t numBits) {
        #define FILE_SPLITTER_BITS_CASE(bits) \
            case (bits): return _process<bits>(inData);

        switch (numBits) {
            FILE_SPLITTER_BITS_CASE(8);
            FILE_SPLITTER_BITS_CASE(9);
            FILE_SPLITTER_BITS_CASE(10);
            FILE_SPLITTER_BITS_CASE(11);
            FILE_SPLITTER_BITS_CASE(12);
            FILE_SPLITTER_BITS_CASE(13);
            FILE_SPLITTER_BITS_CASE(14);
            FILE_SPLITTER_BITS_CASE(15);
            FILE_SPLITTER_BITS_CASE(16);
            FILE_SPLITTER_BITS_CASE(17);
            FILE_SPLITTER_BITS_CASE(18);
            FILE_SPLITTER_BITS_CASE(19);
            FILE_SPLITTER_BITS_CASE(20);
            FILE_SPLITTER_BITS_CASE(21);
            FILE_SPLITTER_BITS_CASE(22);
            FILE_SPLITTER_BITS_CASE(23);
            FILE_SPLITTER_BITS_CASE(24);
            FILE_SPLITTER_BITS_CASE(25);
            FILE_SPLITTER_BITS_CASE(26);
            FILE_SPLITTER_BITS_CASE(27);
            FILE_SPLITTER_BITS_CASE(28);
            FILE_SPLITTER_BITS_CASE(29);
            FILE_SPLITTER_BITS_CASE(30);
            FILE_SPLITTER_BITS_CASE(31);
            FILE_SPLITTER_BITS_CASE(32);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }

        #undef FILE_SPLITTER_BITS_CASE
    }

private:
    template <std::uint8_t bitsNum>
    static Ret _process(const std::span<const std::byte>& inData) {
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
};

#endif  // APPLIB_ORD_AND_TAIL_SPLITTER