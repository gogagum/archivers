#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/range/irange.hpp>

#include <iostream>
#include <cstdint>
#include <limits>

#include "ranges_calc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
class ArithmeticDecoder : RangesCalc {
public:

    ArithmeticDecoder() : _currRange{ 0, RC::total } {}

    /**
     * @brief decode
     * @param source
     * @param wordsCount
     * @param bitsLimit
     * @return
     */
    template <class DictT,
              class SourceT,
              std::output_iterator<typename DictT::Word> OutIterT>
    void decode(
            SourceT& source,
            DictT& dict,
            OutIterT outIter,
            std::size_t wordsCount,
            std::size_t bitsLimit = std::numeric_limits<std::size_t>::max());

private:

    using RC = RangesCalc;
    using OrdRange = typename RC::Range;

private:

    OrdRange _currRange;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class DictT,
          class SourceT,
          std::output_iterator<typename DictT::Word> OutIterT>
void ArithmeticDecoder::decode(SourceT& source,
                               DictT& dict,
                               OutIterT outIter,
                               std::size_t wordsCount,
                               std::size_t bitsLimit) {
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

    int lastPercent = -1;

    for (auto i : boost::irange<std::size_t>(0, wordsCount)) {
        if (int currPercent = (100 * i) / wordsCount;
                currPercent != lastPercent) {
            std::cerr << currPercent << '%' << std::endl;
            lastPercent = currPercent;
        }

        const auto range128 = bm::uint128_t(_currRange.high - _currRange.low);
        const auto dictTotalWords128 = bm::uint128_t(dict.getTotalWordsCnt());
        const auto offset128 = bm::uint128_t(value - _currRange.low + 1);

        const auto aux128 = (offset128 * dictTotalWords128 - 1) / range128;
        const auto aux = aux128.convert_to<std::uint64_t>();
        const auto word = dict.getWord(aux);
        *outIter = word;
        ++outIter;

        auto [low, high, total] = dict.getProbabilityStats(word);
        _currRange = RC::rangeFromStatsAndPrev(_currRange, low, high, total);

        while (true) {
            if (_currRange.high <= RC::half) {
                bool bit = takeBitLimited();
                value = value * 2 + (bit ? 1 : 0);
            } else if (_currRange.low >= RC::half) {
                bool bit = takeBitLimited();
                value = value * 2 - RC::total + (bit ? 1 : 0);
            } else if (_currRange.low >= RC::quater
                       && _currRange.high <= RC::threeQuaters) {
                bool bit = takeBitLimited();
                value = value * 2 - RC::half + (bit ? 1 : 0);
            } else {
                break;
            }
            _currRange = RC::recalcRange(_currRange);
        }
    }

    std::cerr << "100%" << std::endl;
}

}  // namespace ga

#endif // ARITHMETIC_DECODER_HPP

