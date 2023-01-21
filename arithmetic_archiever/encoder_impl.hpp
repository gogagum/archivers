#ifndef ENCODER_IMPL_HPP
#define ENCODER_IMPL_HPP

#include "arithmetic_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAdaptiveEncodeImpl class
///
template <std::uint8_t bytesNum>
struct FileBytesAdaptiveEncodeImpl
        : public BaseAdaptiveEncodeImpl<std::uint16_t{bytesNum} * 8> {
    using Base = BaseAdaptiveEncodeImpl<std::uint16_t{bytesNum} * 8>;
    static void process(FileOpener& fileOpener, std::uint64_t ratio) {
        auto flow = BytesFlow<bytesNum>(fileOpener.getInData());
        auto coder = BytesCoder<bytesNum>(
            flow, [ratio]() { return BytesDict<bytesNum>(ratio); });
        Base::processImpl(fileOpener, flow.getTail(), coder);
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct FileBitsAdaptiveEncodeImpl : public BaseAdaptiveEncodeImpl<bitsNum> {
    using Base = BaseAdaptiveEncodeImpl<bitsNum>;
    static void process(FileOpener& fileOpener, std::uint64_t ratio) {
        auto flow = BitsFlow<bitsNum>(fileOpener.getInData());
        auto coder = BitsCoder<bitsNum>(
                    flow, [ratio]() { return BitsDict<bitsNum>(ratio); });
        Base::processImpl(fileOpener, flow.getTail(), coder);
    }
};

#endif // ENCODER_IMPL_HPP
