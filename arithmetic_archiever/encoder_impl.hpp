#ifndef ENCODER_IMPL_HPP
#define ENCODER_IMPL_HPP

#include "arithmetic_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAdaptiveEncodeImpl class
///
template <std::uint8_t bytesNum>
struct FileBytesAdaptiveEncodeImpl {
    static void process(FileOpener& fileOpener, std::uint64_t ratio) {
        auto flow = BytesFlow<bytesNum>(fileOpener.getInData());
        auto coder = BytesCoder<bytesNum>(flow, BytesDict<bytesNum>(ratio));
        auto tail = flow.getTail();
        auto encoded = ga::ByteDataConstructor();
        encoded.putT<std::uint16_t>(bytesNum * 8);
        encoded.putT<std::uint16_t>(tail.size());
        encoded.putT<std::uint64_t>(ratio);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        coder.encode(encoded);
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct FileBitsAdaptiveEncodeImpl {
    static void process(FileOpener& fileOpener, std::uint64_t ratio) {
        auto flow = BitsFlow<bitsNum>(fileOpener.getInData());
        auto coder = BitsCoder<bitsNum>(flow, BitsDict<bitsNum>(ratio));
        auto tail = flow.getTail();
        auto encoded = ga::ByteDataConstructor();
        encoded.putT<std::uint16_t>(bitsNum);
        encoded.putT<std::uint16_t>(tail.size());
        encoded.putT<std::uint64_t>(ratio);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        coder.encode(encoded);
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    }
};

#endif // ENCODER_IMPL_HPP
