#ifndef ENCODE_IMPL_HPP
#define ENCODE_IMPL_HPP

#include "arithmetic_a_archiever_include.hpp"
#include "../common.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct AAdaptiveEncodeImpl : public BaseAdaptiveEncodeImpl<bitsNum> {
    using Base = BaseAdaptiveEncodeImpl<bitsNum>;
    static void process(FileOpener& fileOpener, optout::OptOstreamRef os) {
        auto flow = Flow<bitsNum>(fileOpener.getInData());
        auto dict = Dict<bitsNum>();
        auto coder = ga::ArithmeticCoder(flow);
        Base::processImpl(fileOpener, flow.getTail(), coder, dict, os);
    }
};

#endif // ENCODE_IMPL_HPP
