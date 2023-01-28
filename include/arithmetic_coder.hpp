#pragma once

#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <iostream>
#include <cstdint>
#include <boost/timer/progress_display.hpp>

#include "flow/traits.hpp"
#include "byte_data_constructor.hpp"
#include "ranges_calc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT>
class ArithmeticCoder : RangesCalc {
public:

    using Word = ga::fl::traits::WordT<FlowT>;

    struct EncodeRet {
        std::size_t wordsCount;
        std::size_t bitsEncoded;
    };

private:
    using RC = RangesCalc;
    using OrdRange = typename RC::Range;
public:

    /**
     * @brief ArithmeticCoder
     * @param byteFlow
     * @param constructor
     */
    ArithmeticCoder(FlowT& byteFlow);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    template <class DictT>
    EncodeRet encode(ByteDataConstructor& dataConstructor,
                     DictT& dict,
                     std::optional<std::reference_wrapper<std::ostream>> os = std::nullopt);

private:
    FlowT& _symFlow;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT>
ArithmeticCoder<FlowT>::ArithmeticCoder(FlowT& symbolsFlow) :
    _symFlow(symbolsFlow) {}

//----------------------------------------------------------------------------//
template <class FlowT>
template <class DictT>
auto ArithmeticCoder<FlowT>::encode(
        ByteDataConstructor& dataConstructor,
        DictT& dict,
        std::optional<std::reference_wrapper<std::ostream>> os) -> EncodeRet {
    auto ret = EncodeRet();
    auto currRange = OrdRange { 0, RC::total };

    std::size_t btf = 0;

    auto barOpt = std::optional<boost::timer::progress_display>();
    if (os.has_value()) {
        barOpt.emplace(_symFlow.size(), os.value(), "");
    }

    for (auto sym : _symFlow) {
        const auto [low, high, total] = dict.getProbabilityStats(sym);
        currRange = RC::rangeFromStatsAndPrev(currRange, low, high, total);

        while (true) {
            if (currRange.high <= RC::half) {
                ret.bitsEncoded += btf + 1;
                dataConstructor.putBit(false);
                dataConstructor.putBitsRepeatWithReset(true, btf);
            } else if (currRange.low >= RC::half) {
                ret.bitsEncoded += btf + 1;
                dataConstructor.putBit(true);
                dataConstructor.putBitsRepeatWithReset(false, btf);
            } else if (currRange.low >= RC::quater
                       && currRange.high <= RC::threeQuaters) {
                ++btf;
            } else {
                break;
            }
            auto lastRange = currRange;

            currRange = RC::recalcRange(currRange);
        }
        ++ret.wordsCount;
        if (barOpt.has_value()) {
            ++barOpt.value();
        }
    }

    ret.bitsEncoded += btf + 2;
    if (currRange.low < RC::quater) {
        dataConstructor.putBit(false);
        dataConstructor.putBitsRepeat(true, btf + 1);
    } else {
        dataConstructor.putBit(true);
        dataConstructor.putBitsRepeat(false, btf + 1);
    }

    return ret;
}

}  // namespace ga

#endif // ARITHMETIC_CODER_HPP
