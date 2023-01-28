#ifndef ENCODE_IMPL_HPP
#define ENCODE_IMPL_HPP

#include "arithmetic_d_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAdaptiveEncodeImpl class
///
template <std::uint8_t bytesNum>
struct FileBytesDAdaptiveEncodeImpl
        : public BaseAdaptiveEncodeImpl<std::uint16_t{bytesNum} * 8> {
    using Base = BaseAdaptiveEncodeImpl<std::uint16_t{bytesNum} * 8>;
    static void process(FileOpener& fileOpener,
                        std::optional<std::reference_wrapper<std::ostream>> os) {
        auto flow = BytesFlow<bytesNum>(fileOpener.getInData());
        auto dict = BytesDict<bytesNum>();
        auto coder = BytesCoder<bytesNum>(flow);
        Base::processImpl(fileOpener, flow.getTail(), coder, dict, os);
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsDAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct FileBitsDAdaptiveEncodeImpl : public BaseAdaptiveEncodeImpl<bitsNum> {
    using Base = BaseAdaptiveEncodeImpl<bitsNum>;
    static void process(FileOpener& fileOpener,
                        std::optional<std::reference_wrapper<std::ostream>> os) {
        auto flow = BitsFlow<bitsNum>(fileOpener.getInData());
        auto dict = BitsDict<bitsNum>();
        auto coder = BitsCoder<bitsNum>(flow);
        Base::processImpl(fileOpener, flow.getTail(), coder, dict, os);
    }
};

#endif // ENCODE_IMPL_HPP
