#pragma once

#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>
#include <bitset>
#include <boost/range/combine.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "byte_data_constructor.hpp"
#include "ranges_calc.hpp"

namespace ga {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT, class DictT, std::uint16_t rangeBits>
class ArithmeticCoder : RangesCalc<rangeBits> {
public:

    using Word = typename FlowT::Word;

    struct EncodeRet {
        std::size_t wordsCount;
        std::size_t bitsEncoded;
    };

private:
    using RC = RangesCalc<rangeBits>;
    using OrdRange = typename RC::Range;
public:

    /**
     * @brief ArithmeticCoder
     * @param byteFlow
     * @param constructor
     */
    ArithmeticCoder(FlowT& byteFlow, DictT&& dict);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    EncodeRet encode(ByteDataConstructor& dataConstructor);

private:
    FlowT& _symFlow;
    DictT _dict;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, class DictT, std::uint16_t rangeBits>
ArithmeticCoder<FlowT, DictT, rangeBits>::ArithmeticCoder(FlowT& symbolsFlow,
                                               DictT&& dict) :
    _symFlow(symbolsFlow),
    _dict(std::forward<DictT>(dict)) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, std::uint16_t rangeBits>
auto ArithmeticCoder<FlowT, DictT, rangeBits>::encode(
        ByteDataConstructor& dataConstructor) -> EncodeRet {
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

        const auto range = bm::uint128_t(currRange.high - currRange.low);
        const auto [low, high, total] = _dict.getProbabilityStats(sym);

        const auto low128 = bm::uint128_t(low);
        const auto high128 = bm::uint128_t(high);
        const auto total128 = bm::uint128_t(total);

        currRange = OrdRange {
            currRange.low + ((range * low128) / total128).template convert_to<std::uint64_t>(),
            currRange.low + ((range * high128) / total128).template convert_to<std::uint64_t>()
        };

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
