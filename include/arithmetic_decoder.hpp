#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/range/irange.hpp>

#include <iostream>
#include <cstdint>
#include <limits>
#include <vector>

#include "data_parser.hpp"
#include "ranges_calc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class DictT>
class ArithmeticDecoder : RangesCalc {
public:
    using Source = DataParser;
    using Ret = std::vector<typename DictT::Word>;

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

    using RC = RangesCalc;
    using OrdRange = typename RC::Range;

private:
    DictT _dict;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class DictT>
ArithmeticDecoder<DictT>::ArithmeticDecoder(DictT&& dict)
    : _dict(std::forward<DictT>(dict)) {}

//----------------------------------------------------------------------------//
template <class DictT>
auto
ArithmeticDecoder<DictT>::decode(Source& source,
                                 std::size_t wordsCount,
                                 std::size_t bitsLimit) -> Ret {
    const auto takeBitLimited = [&source, &bitsLimit]() -> bool {
        if (bitsLimit == 0) {
            return false;
        }
        --bitsLimit;
        return source.takeBit();
    };

    typename RC::Count value = 0;

    for (auto _ : boost::irange<std::size_t>(0, RC::numBits)) {
        value = (value << 1) + (takeBitLimited() ? 1 : 0);
    }

    auto currRange = OrdRange { 0, RC::total };

    Ret ret;

    int lastPercent = -1;

    for (auto i : boost::irange<std::size_t>(0, wordsCount)) {
        if (int currPercent = (100 * i) / wordsCount;
                currPercent != lastPercent) {
            std::cerr << currPercent << '%' << std::endl;
            lastPercent = currPercent;
        }

        const auto range128 = bm::uint128_t(currRange.high - currRange.low);
        const auto dictTotalWords128 = bm::uint128_t(_dict.getTotalWordsCnt());
        const auto offset128 = bm::uint128_t(value - currRange.low + 1);

        const auto aux128 = (offset128 * dictTotalWords128 - 1) / range128;
        const auto aux = aux128.convert_to<std::uint64_t>();
        const auto sym = _dict.getWord(aux);
        ret.push_back(sym);

        auto [low, high, total] = _dict.getProbabilityStats(sym);
        currRange = RC::rangeFromStatsAndPrev(currRange, low, high, total);

        while (true) {
            if (currRange.high <= RC::half) {
                bool bit = takeBitLimited();
                value = value * 2 + (bit ? 1 : 0);
            } else if (currRange.low >= RC::half) {
                bool bit = takeBitLimited();
                value = value * 2 - RC::total + (bit ? 1 : 0);
            } else if (currRange.low >= RC::quater
                       && currRange.high <= RC::threeQuaters) {
                bool bit = takeBitLimited();
                value = value * 2 - RC::half + (bit ? 1 : 0);
            } else {
                break;
            }
            currRange = RC::recalcRange(currRange);
        }
    }

    std::cerr << "100%" << std::endl;
    return ret;
}

}  // namespace ga

#endif // ARITHMETIC_DECODER_HPP

