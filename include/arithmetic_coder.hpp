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
#include "dictionary/static_dicitionary.hpp"
#include "ranges_calc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT, typename CountT = std::uint32_t>
class ArithmeticCoder : RangesCalc<typename FlowT::Sym> {
public:

    using Sym = typename FlowT::Sym;

    using Res = ArithmeticCoderEncoded;

    using RangesCalc<Sym>::symsNum;
    using RangesCalc<Sym>::symsNum_2;
    using RangesCalc<Sym>::symsNum_4;
    using RangesCalc<Sym>::symsNum_3to4;
    using RangesCalc<Sym>::correctingConst;

public:

    /**
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
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

    std::uint64_t _computeCorrectingConst() const;

private:
    FlowT _symFlow;
    dict::StaticDictionary<Sym> _dict;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
ArithmeticCoder<FlowT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow),
          _dict(_countSyms()) {}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
auto ArithmeticCoder<FlowT, CountT>::encode() -> Res {
    auto ret = Res();

    _serializeNumBytes(ret);
    _serializeTail(ret);
    _dict.template serialize<CountT>(ret);

    auto currRange = typename RangesCalc<Sym>::Range {
        0, RangesCalc<Sym>::symsNum * correctingConst
    };

    std::size_t btf = 0;

    for (auto sym : _symFlow) {
        std::uint64_t range = currRange.high - currRange.low;

        auto h = _dict.getHigherCumulativeNumFound(sym);
        auto l = _dict.getLowerCumulativeNumFound(sym);

        currRange = typename RangesCalc<Sym>::Range{
            currRange.low + (range * l) / _dict.totalWordsCount(),
            currRange.low + (range * h) / _dict.totalWordsCount()
        };

        while (true) {
            if (currRange.high <= symsNum_2 * correctingConst) {
                ret.putBit(false);
                ret.putBitsRepeatWithReset(true, btf);
            } else if (currRange.low >= symsNum_2 * correctingConst) {
                ret.putBit(true);
                ret.putBitsRepeatWithReset(false, btf);
            } else if (currRange.low >= symsNum_4 * correctingConst
                       && currRange.high <= symsNum_3to4 * correctingConst) {
                ++btf;
            } else {
                break;
            }
            currRange = RangesCalc<Sym>::recalcRange(currRange);
        }
    }

    if (currRange.low < symsNum_4 * correctingConst) {
        ret.putBit(false);
        ret.putBitsRepeat(true, btf + 1);
    } else {
        ret.putBit(true);
        ret.putBitsRepeat(false, btf + 1);
    }

    return ret;
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
std::vector<std::uint64_t> ArithmeticCoder<FlowT, CountT>::_countSyms() {
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
template <class FlowT, typename CountT>
void ArithmeticCoder<FlowT, CountT>::_serializeNumBytes(Res& res) {
    constexpr std::uint8_t numBytes = Sym::numBytes;
    res.putT<std::uint8_t>(numBytes);
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void ArithmeticCoder<FlowT, CountT>::_serializeTail(Res& res) {
    auto tailSize = static_cast<uint8_t>(_symFlow.getTailSize());
    res.putT<std::uint8_t>(tailSize);

    auto tail = _symFlow.getTail();
    for (auto tailByte : tail) {
        res.putByte(tailByte);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
std::uint64_t
ArithmeticCoder<FlowT, CountT>::_computeCorrectingConst() const {
    std::uint64_t correctingConst = 1;
    while (correctingConst * symsNum < (1ull << 40)) {
        correctingConst <<= 1;
    }
    return correctingConst;
}

}  // namespace ga

#endif // ARITHMETIC_CODER_HPP
