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
#include "dictionary/traits.hpp"

namespace ga {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT, class DictT, typename CountT = std::uint32_t>
class ArithmeticCoder : RangesCalc<typename FlowT::Word> {
public:

    using Word = typename FlowT::Word;

    using Res = ByteDataConstructor;

private:
    using RangesCalc<Word>::correctedSymsNum;
    using RangesCalc<Word>::correctedSymsNum_2;
    using RangesCalc<Word>::correctedSymsNum_4;
    using RangesCalc<Word>::correctedSymsNum_3to4;
    using OrdRange = typename RangesCalc<Word>::Range;
public:

    /**
     * @brief ArithmeticCoder
     * @param byteFlow
     * @param constructor
     */
    template <class DictConstructor>
    ArithmeticCoder(FlowT& byteFlow, DictConstructor&& constructor);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    void encode(ByteDataConstructor& dataConstructor);

private:

    void _serializeDict(ByteDataConstructor& res);

    void _serializeFileWordsCount(ByteDataConstructor& res);

private:
    FlowT& _symFlow;
    DictT _dict;
    CountT _fileWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, class DictT, class CountT>
template <class DictConstructor>
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT& symbolsFlow,
                                                       DictConstructor&& constructor) :
    _symFlow(symbolsFlow),
    _dict(constructor()),
    _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::encode(ByteDataConstructor& dataConstructor) {
    _serializeDict(dataConstructor);
    _serializeFileWordsCount(dataConstructor);

    auto currRange = OrdRange { 0, correctedSymsNum };

    std::size_t btf = 0;

    std::int8_t lastPercent = -1;
    std::size_t wordsCoded = 0;

    for (auto sym : _symFlow) {
        if (std::uint8_t currPercent = wordsCoded * 100 / _symFlow.getNumberOfWords();
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
            if (currRange.high <= correctedSymsNum_2) {
                dataConstructor.putBit(false);
                dataConstructor.putBitsRepeatWithReset(true, btf);
            } else if (currRange.low >= correctedSymsNum_2) {
                dataConstructor.putBit(true);
                dataConstructor.putBitsRepeatWithReset(false, btf);
            } else if (currRange.low >= correctedSymsNum_4
                       && currRange.high <= correctedSymsNum_3to4) {
                ++btf;
            } else {
                break;
            }
            currRange = RangesCalc<Word>::recalcRange(currRange);
        }

        ++wordsCoded;
    }

    std::cerr << "100%" << std::endl;

    if (currRange.low < correctedSymsNum_4) {
        dataConstructor.putBit(false);
        dataConstructor.putBitsRepeat(true, btf + 1);
    } else {
        dataConstructor.putBit(true);
        dataConstructor.putBitsRepeat(false, btf + 1);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeDict(Res& res) {
    if constexpr (ga::dict::traits::needSerialize<DictT, ga::ByteDataConstructor>) {
        _dict.serialize(res);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeFileWordsCount(Res& res) {
    std::cerr << "Words count: " << _fileWordsCount << std::endl;
    res.putT<CountT>(_fileWordsCount);
}

}  // namespace ga

#endif // ARITHMETIC_CODER_HPP
