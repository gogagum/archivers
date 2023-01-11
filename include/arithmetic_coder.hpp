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
class ArithmeticCoder : RangesCalc<typename FlowT::Sym> {
public:

    using Sym = typename FlowT::Sym;

    using Res = ByteDataConstructor;

private:
    using RangesCalc<Sym>::correctedSymsNum;
    using RangesCalc<Sym>::correctedSymsNum_2;
    using RangesCalc<Sym>::correctedSymsNum_4;
    using RangesCalc<Sym>::correctedSymsNum_3to4;
    using OrdRange = typename RangesCalc<Sym>::Range;
public:

    /**
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
    template <class DictT_ = DictT> requires std::is_same_v<typename DictT_::ConstructionTag, dict::tags::ConstructsFromSymsCounts>
    ArithmeticCoder(FlowT&& byteFlow);

    /**
     * @brief ArithmeticCoder
     * @param byteFlow
     */
    template <class DictT_ = DictT> requires std::is_same_v<typename DictT_::ConstructionTag, dict::tags::ConstructsFromNoArgs>
    ArithmeticCoder(FlowT&& byteFlow);

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
template <class DictT_> requires std::is_same_v<typename DictT_::ConstructionTag, dict::tags::ConstructsFromSymsCounts>
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow),
          _dict(_countSyms()),
          _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
template <class DictT_> requires std::is_same_v<typename DictT_::ConstructionTag, dict::tags::ConstructsFromNoArgs>
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow),
          _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
auto ArithmeticCoder<FlowT, DictT, CountT>::encode() -> Res {
    auto ret = Res();

    _serializeNumBits(ret);
    _serializeTail(ret);
    _serializeFileWordsCount(ret);

    if constexpr (std::is_same_v<typename DictT::ConstructionTag, dict::tags::ConstructsFromSymsCounts>) {
        _dict.template serialize<CountT>(ret);
    }

    auto currRange = OrdRange { 0, correctedSymsNum };

    std::size_t btf = 0;

    for (auto sym : _symFlow) {
        std::uint64_t range = currRange.high - currRange.low;

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
            currRange = RangesCalc<Sym>::recalcRange(currRange);
        }
    }

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
    std::vector<std::uint64_t> numFound(Sym::wordsCount, 0);
    for (auto word: _symFlow) {
        numFound[Sym::ord(word)]++;
    }
    std::vector<std::uint64_t> numFoundCumulative(Sym::wordsCount, 0);
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
    res.putT<std::uint16_t>(Sym::numBits);
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeTail(Res& res) {
    auto tail = _symFlow.getTail();
    res.putT<std::uint8_t>(tail.size());

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
