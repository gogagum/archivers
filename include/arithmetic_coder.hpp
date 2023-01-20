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

#include "byte_data_constructor.hpp"
#include "ranges_calc.hpp"
#include "dictionary/traits.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT, class DictT, typename CountT = std::uint32_t, std::uint16_t minSymsNumBits = 33>
class ArithmeticCoder : RangesCalc<typename FlowT::Word, minSymsNumBits> {
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
    ArithmeticCoder(FlowT&& byteFlow, DictConstructor&& constructor);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    Res encode();

private:

    std::vector<std::uint64_t> _countSyms();

    void _serializeNumBits(Res& res);

    void _serializeDict(Res& res);

    void _serializeTail(Res& res);

    void _serializeFileWordsCount(Res& res);

private:
    FlowT _symFlow;
    DictT _dict;
    CountT _fileWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, class DictT, class CountT, std::uint16_t minSymsNumBits>
template <class DictConstructor>
ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::ArithmeticCoder(FlowT&& symbolsFlow,
                                                       DictConstructor&& constructor) :
    _symFlow(symbolsFlow),
    _dict(constructor()),
    _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT, std::uint16_t minSymsNumBits>
auto ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::encode() -> Res {
    auto ret = Res();

    _serializeNumBits(ret);
    _serializeDict(ret);
    _serializeFileWordsCount(ret);
    _serializeTail(ret);

    auto currRange = OrdRange { 0, correctedSymsNum };

    std::size_t btf = 0;

    std::int8_t lastPercent = -1;
    std::size_t wordsCoded = 0;

    for (auto sym : _symFlow) {
        if (std::uint8_t currPercent = wordsCoded * 100 / _symFlow.getNumberOfWords();
                currPercent != lastPercent) {
            std::cout << static_cast<int>(currPercent) << "%" << std::endl;
            lastPercent = currPercent;
        }

        auto range = currRange.high - currRange.low;
        auto [low, high, totalWords] = _dict.getProbabilityStats(sym);

        currRange = OrdRange {
            currRange.low + (range * low) / totalWords,
            currRange.low + (range * high) / totalWords
        };

        while (true) {
            if (currRange.high <= correctedSymsNum_2) {
                ret.putBit(false);
                ret.putBitsRepeatWithReset(true, btf);
            } else if (currRange.low >= correctedSymsNum_2) {
                ret.putBit(true);
                ret.putBitsRepeatWithReset(false, btf);
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

    std::cout << "100%" << std::endl;

    if (currRange.low < correctedSymsNum_4) {
        ret.putBit(false);
        ret.putBitsRepeat(true, btf + 1);
    } else {
        ret.putBit(true);
        ret.putBitsRepeat(false, btf + 1);
    }

    return ret;
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT, std::uint16_t minSymsNumBits>
std::vector<std::uint64_t> ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::_countSyms() {
    std::vector<std::uint64_t> numFound(Word::wordsCount, 0);
    for (auto word: _symFlow) {
        numFound[Word::ord(word)]++;
    }
    std::vector<std::uint64_t> numFoundCumulative(Word::wordsCount, 0);
    std::uint64_t currCumulativeNumFound = 0;
    for (auto&& [numFound, numFoundCumulative]
         : boost::range::combine(numFound, numFoundCumulative)) {
        currCumulativeNumFound += numFound;
        numFoundCumulative = currCumulativeNumFound;
    }
    return numFoundCumulative;
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT, std::uint16_t minSymsNumBits>
void ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::_serializeNumBits(Res& res) {
    res.putT<std::uint16_t>(Word::numBits);
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT, std::uint16_t minSymsNumBits>
void ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::_serializeDict(Res& res) {
    if constexpr (ga::dict::traits::needSerialize<DictT, ga::ByteDataConstructor>) {
        _dict.serialize(res);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT, std::uint16_t minSymsNumBits>
void ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::_serializeTail(Res& res) {
    auto tail = _symFlow.getTail();
    res.putT<std::uint16_t>(tail.size());

    for (auto tailBit : tail) {
        res.putBit(tailBit);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT, std::uint16_t minSymsNumBits>
void ArithmeticCoder<FlowT, DictT, CountT, minSymsNumBits>::_serializeFileWordsCount(Res& res) {
    res.putT<CountT>(_fileWordsCount);
}

}  // namespace ga

#endif // ARITHMETIC_CODER_HPP
