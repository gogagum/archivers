#ifndef ENCODE_IMPL_HPP
#define ENCODE_IMPL_HPP

#include "ppmd_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsDAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct PPMDEncodeImpl : public BaseAdaptiveEncodeImpl<bitsNum> {
    static void process(FileOpener& fileOpener,
                        std::uint16_t contextLength,
                        Dict& dict,
                        optout::OptOstreamRef os) {
        auto flow = Flow<bitsNum>(fileOpener.getInData());
        auto coder = ga::ArithmeticCoder(flow);
        auto tail = flow.getTail();
        auto encoded = ga::ByteDataConstructor();
        encoded.putT<std::uint16_t>(bitsNum);
        encoded.putT<std::uint16_t>(tail.size());
        encoded.putT<std::uint8_t>(contextLength);
        const auto wordsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        const auto bitsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        auto [wordsCount, bitsCount] = coder.encode(encoded, dict, os);
        encoded.putTToPosition(wordsCount, wordsCountPos);
        encoded.putTToPosition(bitsCount, bitsCountPos);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    }
};

#endif // ENCODE_IMPL_HPP
