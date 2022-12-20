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

#include "arithmetic_coder_encoded.hpp"
#include "ranges_calc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT, class DictT, typename CountT = std::uint32_t>
class ArithmeticCoder : RangesCalc<typename FlowT::Sym> {
public:

    using Sym = typename FlowT::Sym;

    using Res = ArithmeticCoderEncoded;

private:
    using RangesCalc<Sym>::correctedSymsNum;
    using RangesCalc<Sym>::correctedSymsNum_2;
    using RangesCalc<Sym>::correctedSymsNum_4;
    using RangesCalc<Sym>::correctedSymsNum_3to4;

public:

    /**
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
    template <class DictT_ = DictT> requires DictT_::requireSymsCounts
    ArithmeticCoder(FlowT&& byteFlow);

    /**
     * @brief ArithmeticCoder
     * @param byteFlow
     */
    template <class DictT_ = DictT> requires DictT_::constructsFromNoArgs
    ArithmeticCoder(FlowT&& byteFlow);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    Res encode();

private:

    std::vector<std::uint64_t> _countSyms();

    void _serializeNumBytes(Res& res);

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
template <class DictT_> requires DictT_::requireSymsCounts
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow),
          _dict(_countSyms()),
          _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
template <class DictT_> requires DictT_::constructsFromNoArgs
ArithmeticCoder<FlowT, DictT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow),
          _fileWordsCount(static_cast<CountT>(_symFlow.getNumberOfWords())) {}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
auto ArithmeticCoder<FlowT, DictT, CountT>::encode() -> Res {
    auto ret = Res();

    _serializeNumBytes(ret);
    _serializeTail(ret);
    _serializeFileWordsCount(ret);

    if constexpr (DictT::requireSymsCounts) {
        _dict.template serialize<CountT>(ret);
    }

    auto currRange = typename RangesCalc<Sym>::Range { 0, correctedSymsNum };

    std::size_t btf = 0;

    for (auto sym : _symFlow) {
        std::uint64_t range = currRange.high - currRange.low;

        auto h = _dict.getHigherCumulativeNumFound(sym);
        auto l = _dict.getLowerCumulativeNumFound(sym);

        currRange = typename RangesCalc<Sym>::Range{
            currRange.low + (range * l) / _dict.totalWordsCount(),
            currRange.low + (range * h) / _dict.totalWordsCount()
        };

        if constexpr (DictT::supportsIncrease) {
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
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeNumBytes(Res& res) {
    constexpr std::uint8_t numBytes = Sym::numBytes;
    res.putT<std::uint8_t>(numBytes);
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeTail(Res& res) {
    auto tailSize = static_cast<uint8_t>(_symFlow.getTailSize());
    res.putT<std::uint8_t>(tailSize);

    auto tail = _symFlow.getTail();
    for (auto tailByte : tail) {
        res.putByte(tailByte);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, class DictT, typename CountT>
void ArithmeticCoder<FlowT, DictT, CountT>::_serializeFileWordsCount(Res& res) {
    res.putT<CountT>(_fileWordsCount);
}


}  // namespace ga

#endif // ARITHMETIC_CODER_HPP
