#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/range/irange.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>
#include <map>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>

#include "data_parser.hpp"
#include "ranges_calc.hpp"

namespace ga {

namespace bc = boost::container;
namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class SymT, class DictT, typename CountT = std::uint32_t>
class ArithmeticDecoder : RangesCalc<SymT> {
public:
    using Source = DataParser;
    using Ret = std::vector<SymT>;

public:

    template <class DictConstructor>
    ArithmeticDecoder(Source& source, DictConstructor&& constructor);

    /**
     * @brief decode - decode source as a vector of bytes.
     * @return
     */
    Ret decode();

private:

    using RangesCalc<SymT>::symsNum;
    using RangesCalc<SymT>::correctedSymsNum;
    using RangesCalc<SymT>::correctedSymsNum_2;
    using RangesCalc<SymT>::correctedSymsNum_4;
    using RangesCalc<SymT>::correctedSymsNum_3to4;
    using OrdRange = typename RangesCalc<SymT>::Range;

private:

    std::vector<std::uint64_t> _deserializeWordsCounts();

    CountT _deserializeFileWordsCount();

    constexpr const static std::uint16_t _additionalBitsCnt =
            RangesCalc<SymT>::_computeAdditionalBitsCnt();

private:
    Source& _source;
    DictT _dict;                                 // #1 to deserialize.
    CountT _fileWordsCount;                      // #2 to deserialize.
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
template <class DictConstructor>
ArithmeticDecoder<SymT, DictT, CountT>::ArithmeticDecoder(
        Source& source, DictConstructor&& constructor)
    : _source(source),
      _dict(constructor()),
      _fileWordsCount(_deserializeFileWordsCount()) {}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
auto ArithmeticDecoder<SymT, DictT, CountT>::decode() -> Ret {
    typename RangesCalc<SymT>::Count value = 0;
    std::size_t valueBits = SymT::numBits + _additionalBitsCnt;

    for (auto _ : boost::irange<std::size_t>(0, valueBits)) {
        value = (value << 1) + (_source.takeBit() ? 1 : 0);
    }

    auto currRange = OrdRange { 0, correctedSymsNum };

    Ret ret;

    int lastPercent = -1;

    for (auto i : boost::irange<CountT>(0, _fileWordsCount)) {
        if (int currPercent = (100 * i) / _fileWordsCount;
                currPercent != lastPercent) {
            std::cerr << currPercent << '%' << std::endl;
            lastPercent = currPercent;
        }

        const auto range = currRange.high - currRange.low;
        const auto range128 = bm::uint128_t(range);
        const auto aux =
                ((value - currRange.low + 1) * _dict.getTotalWordsCount() - 1) / range;

        const auto sym = _dict.getWord(aux);
        ret.push_back(sym);

        auto [low, high, total] = _dict.getProbabilityStats(sym);

        const auto low128 = bm::uint128_t(low);
        const auto high128 = bm::uint128_t(high);
        const auto total128 = bm::uint128_t(total);

        currRange = OrdRange {
            currRange.low + ((range * low128) / total128).template convert_to<std::uint64_t>(),
            currRange.low + ((range * high128) / total128).template convert_to<std::uint64_t>()
        };

        while (true) {
            if (currRange.high <= correctedSymsNum_2) {
                bool bit = _source.takeBit();
                value = value * 2 + (bit ? 1 : 0);
            } else if (currRange.low >= correctedSymsNum_2) {
                bool bit = _source.takeBit();
                value = value * 2 - correctedSymsNum + (bit ? 1 : 0);
            } else if (currRange.low >= correctedSymsNum_4
                       && currRange.high <= correctedSymsNum_3to4) {
                bool bit = _source.takeBit();
                value = value * 2 - correctedSymsNum_2 + (bit ? 1 : 0);
            } else {
                break;
            }
            currRange = RangesCalc<SymT>::recalcRange(currRange);
        }
    }

    std::cerr << "100%" << std::endl;
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
std::vector<std::uint64_t>
ArithmeticDecoder<SymT, DictT, CountT>::_deserializeWordsCounts() {
    std::vector<std::uint64_t> ret(SymT::wordsCount);
    CountT numUniqueSyms = _source.takeT<std::uint32_t>();

    for (auto _ : boost::irange<CountT>(0, numUniqueSyms)) {
        auto sym = _source.takeT<SymT>();
        auto numFound = _source.takeT<CountT>();
        ret[SymT::ord(sym)] = numFound;
    }

    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
CountT ArithmeticDecoder<SymT, DictT, CountT>::_deserializeFileWordsCount() {
    auto wordsCount = _source.takeT<CountT>();
    std::cerr << "Words count: " << wordsCount << std::endl;
    return wordsCount;
}

}  // namespace ga

#endif // ARITHMETIC_DECODER_HPP

