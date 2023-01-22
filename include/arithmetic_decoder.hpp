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
#include <limits>

#include "data_parser.hpp"
#include "ranges_calc.hpp"

namespace ga {

namespace bc = boost::container;
namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class SymT, class DictT>
class ArithmeticDecoder : RangesCalc<SymT> {
public:
    using Source = DataParser;
    using Ret = std::vector<SymT>;

public:

    ArithmeticDecoder(DictT&& dict);

    /**
     * @brief decode
     * @param source
     * @param wordsCount
     * @param bitsLimit
     * @return
     */
    Ret decode(Source& source,
               std::size_t wordsCount,
               std::size_t bitsLimit = std::numeric_limits<std::size_t>::max());

private:

    using RangesCalc<SymT>::symsNum;
    using RangesCalc<SymT>::correctedSymsNum;
    using RangesCalc<SymT>::correctedSymsNum_2;
    using RangesCalc<SymT>::correctedSymsNum_4;
    using RangesCalc<SymT>::correctedSymsNum_3to4;
    using OrdRange = typename RangesCalc<SymT>::Range;

private:

    std::vector<std::uint64_t> _deserializeWordsCounts();

    constexpr const static std::uint16_t _additionalBitsCnt =
            RangesCalc<SymT>::_computeAdditionalBitsCnt();

private:
    DictT _dict;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, class DictT>
ArithmeticDecoder<SymT, DictT>::ArithmeticDecoder(DictT&& dict)
    : _dict(std::forward<DictT>(dict)) {}

//----------------------------------------------------------------------------//
template <class SymT, class DictT>
auto
ArithmeticDecoder<SymT, DictT>::decode(Source& source,
                                       std::size_t wordsCount,
                                       std::size_t bitsLimit) -> Ret {
    const auto takeBitLimited = [&source, &bitsLimit]() -> bool {
        if (bitsLimit == 0) {
            return false;
        }
        --bitsLimit;
        return source.takeBit();
    };

    typename RangesCalc<SymT>::Count value = 0;
    std::size_t valueBits = SymT::numBits + _additionalBitsCnt;

    for (auto _ : boost::irange<std::size_t>(0, valueBits)) {
        value = (value << 1) + (takeBitLimited() ? 1 : 0);
    }

    auto currRange = OrdRange { 0, correctedSymsNum };

    Ret ret;

    int lastPercent = -1;

    for (auto i : boost::irange<std::size_t>(0, wordsCount)) {
        if (int currPercent = (100 * i) / wordsCount;
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
            currRange.low + ((range128 * low128) / total128).template convert_to<std::uint64_t>(),
            currRange.low + ((range128 * high128) / total128).template convert_to<std::uint64_t>()
        };

        while (true) {
            if (currRange.high <= correctedSymsNum_2) {
                bool bit = takeBitLimited();
                value = value * 2 + (bit ? 1 : 0);
            } else if (currRange.low >= correctedSymsNum_2) {
                bool bit = takeBitLimited();
                value = value * 2 - correctedSymsNum + (bit ? 1 : 0);
            } else if (currRange.low >= correctedSymsNum_4
                       && currRange.high <= correctedSymsNum_3to4) {
                bool bit = takeBitLimited();
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

}  // namespace ga

#endif // ARITHMETIC_DECODER_HPP

