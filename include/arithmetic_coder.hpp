#pragma once

#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <iostream>
#include <cstdint>

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
    EncodeRet encode(ByteDataConstructor& dataConstructor, DictT& dict);

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
        ByteDataConstructor& dataConstructor, DictT& dict) -> EncodeRet {
    auto ret = EncodeRet();
    auto currRange = OrdRange { 0, RC::total };

    std::size_t btf = 0;

    std::int8_t lastPercent = -1;
    std::size_t wordsCoded = 0;

    for (auto sym : _symFlow) {
        ++ret.wordsCount;
        if (std::uint8_t currPercent = wordsCoded * 100 / _symFlow.size();
                currPercent != lastPercent) {
            std::cerr << static_cast<int>(currPercent) << "%" << std::endl;
            lastPercent = currPercent;
        }

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

        ++wordsCoded;
    }

    std::cerr << "100%" << std::endl;

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
