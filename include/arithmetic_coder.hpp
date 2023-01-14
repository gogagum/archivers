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
#include "dictionary/dictionary_tags.hpp"
#include "dictionary/traits.hpp"

namespace ga {

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
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
    template <class DictT_, class... DictOuterArgs>
        requires ga::dict::traits::constructionTypeIs<
            DictT_,
            dict::tags::NeedWordsCounts
        >
    ArithmeticCoder(FlowT&& byteFlow, DictOuterArgs&&... dictOuterArgs);

    /**
     * @brief ArithmeticCoder
     * @param byteFlow
     */
    template <class DictT_ = DictT, class... DictOuterArgs>
        requires ga::dict::traits::constructionTypeIs<
            DictT_,
            dict::tags::NoNeedWordsCounts
        >
    ArithmeticCoder(FlowT&& byteFlow, DictOuterArgs&&... dictOuterArgs);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    Res encode();

private:

    std::vector<std::uint64_t> _countSyms();

    void _serializeNumBits(Res& res);

    void _serializeTail(Res& res);

    void _serializeFileWordsCount(Res& res);

private:
    FlowT _symFlow;
    DictT _dict;
    CountT _fileWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
template <class DictT_, class... DictOuterArgs>
    requires ga::dict::traits::constructionTypeIs<
        DictT_,
        dict::tags::NeedWordsCounts
    >
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow,
                                                       DictOuterArgs&&... dictOuterArgs)
        : _symFlow(symbolsFlow),
          _dict(_countSyms(), std::forward<DictOuterArgs>(dictOuterArgs)...),
          _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
template <class DictT_, class... DictOuterArgs>
    requires ga::dict::traits::constructionTypeIs<
        DictT_,
        dict::tags::NoNeedWordsCounts
    >
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow,
                                                       DictOuterArgs&&... dictOuterArgs)
        : _symFlow(symbolsFlow),
          _dict(std::forward<DictOuterArgs>(dictOuterArgs)...),
          _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
auto ArithmeticCoder<FlowT, DictT, CountT>::encode() -> Res {
    auto ret = Res();

    _serializeNumBits(ret);
    _serializeTail(ret);
    _serializeFileWordsCount(ret);

    if constexpr (std::is_same_v<typename DictT::ConstructionTag, dict::tags::NeedWordsCounts>) {
        _dict.template serialize<CountT>(ret);
    }

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

        auto h = _dict.getHigherCumulativeNumFound(sym);
        auto l = _dict.getLowerCumulativeNumFound(sym);

        currRange = OrdRange {
            currRange.low + (range * l) / _dict.totalWordsCount(),
            currRange.low + (range * h) / _dict.totalWordsCount()
        };

        if constexpr (dict::traits::needWordIncrease<DictT>) {
            _dict.increaseWordCount(sym);
        }

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
template <class FlowT, class DictT, typename CountT>
std::vector<std::uint64_t> ArithmeticCoder<FlowT, DictT, CountT>::_countSyms() {
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
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeNumBits(Res& res) {
    res.putT<std::uint16_t>(Word::numBits);
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeTail(Res& res) {
    auto tail = _symFlow.getTail();
    res.putT<std::uint16_t>(tail.size());

    for (auto tailBit : tail) {
        res.putBit(tailBit);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeFileWordsCount(Res& res) {
    res.putT<CountT>(_fileWordsCount);
}

}  // namespace ga

#endif // ARITHMETIC_CODER_HPP
