#ifndef ENCODE_IMPL_HPP
#define ENCODE_IMPL_HPP

#include "arithmetic_a_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAAdaptiveEncodeImpl class
///
template <std::uint8_t bytesNum>
struct FileBytesAAdaptiveEncodeImpl
        : public BaseAdaptiveEncodeImpl<std::uint16_t{bytesNum} * 8> {
    using Base = BaseAdaptiveEncodeImpl<std::uint16_t{bytesNum} * 8>;
    static void process(FileOpener& fileOpener) {
        auto flow = BytesFlow<bytesNum>(fileOpener.getInData());
        auto coder = BytesCoder<bytesNum>(flow, BytesDict<bytesNum>());
        Base::processImpl(fileOpener, flow.getTail(), coder);
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsAAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct FileBitsAAdaptiveEncodeImpl : public BaseAdaptiveEncodeImpl<bitsNum> {
    using Base = BaseAdaptiveEncodeImpl<bitsNum>;
    static void process(FileOpener& fileOpener) {
        auto flow = BitsFlow<bitsNum>(fileOpener.getInData());
        auto coder = BitsCoder<bitsNum>(flow, BitsDict<bitsNum>());
        Base::processImpl(fileOpener, flow.getTail(), coder);
    }
};

#endif // ENCODE_IMPL_HPP
