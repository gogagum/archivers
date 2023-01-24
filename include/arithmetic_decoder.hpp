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
template <class DictT, std::uint16_t rangeBits>
class ArithmeticDecoder : RangesCalc<rangeBits> {
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

    using RC = RangesCalc<rangeBits>;
    using OrdRange = typename RC::Range;

private:
    DictT _dict;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class DictT, std::uint16_t rangeBits>
ArithmeticDecoder<DictT, rangeBits>::ArithmeticDecoder(DictT&& dict)
    : _dict(std::forward<DictT>(dict)) {}

//----------------------------------------------------------------------------//
template <class DictT, std::uint16_t rangeBits>
auto
ArithmeticDecoder<DictT, rangeBits>::decode(Source& source,
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

        const auto range = currRange.high - currRange.low;
        const auto range128 = bm::uint128_t(range);
        const auto dictTotalWords128 = bm::uint128_t(_dict.getTotalWordsCount());
        const auto offset128 = bm::uint128_t(value - currRange.low + 1);

        const auto aux =
                ((offset128 * dictTotalWords128 - 1) / range128).template convert_to<std::uint64_t>();

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

