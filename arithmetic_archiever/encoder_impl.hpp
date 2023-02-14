#ifndef ENCODER_IMPL_HPP
#define ENCODER_IMPL_HPP

#include "arithmetic_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct AdaptiveEncodeImpl {
    static void process(FileOpener& fileOpener,
                        std::uint64_t ratio,
                        optout::OptOstreamRef os) {
        auto flow = Flow<bitsNum>(fileOpener.getInData());
        auto dict = Dict(1 << bitsNum, ratio);
        auto coder = ga::ArithmeticCoder(flow);
        auto tail = flow.getTail();
        auto encoded = ga::ByteDataConstructor();
        encoded.putT<std::uint16_t>(bitsNum);
        encoded.putT<std::uint16_t>(tail.size());
        encoded.putT<std::uint64_t>(ratio);
        const auto wordsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        const auto bitsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        auto [wordsCount, bitsCount] = coder.encode(encoded, dict, os);
        encoded.putTToPosition(wordsCount, wordsCountPos);
        encoded.putTToPosition(bitsCount, bitsCountPos);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    }
};

#endif // ENCODER_IMPL_HPP
