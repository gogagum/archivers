#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/range/irange.hpp>
#include <boost/timer/progress_display.hpp>

#include <iostream>
#include <cstdint>
#include <limits>
#include <optional>

#include "ranges_calc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
class ArithmeticDecoder : RangesCalc {
public:

    ArithmeticDecoder() : _currRange{ 0, RC::total } {}

    template <class WordT>
    void decode(
            auto& source,
            auto& dict,
            auto outIter,
            std::size_t wordsCount,
            std::size_t bitsLimit = std::numeric_limits<std::size_t>::max(),
            std::optional<std::reference_wrapper<std::ostream>> os = std::nullopt
            ) requires std::output_iterator<decltype(outIter), WordT>;

private:

    using RC = RangesCalc;
    using OrdRange = typename RC::Range;

private:

    OrdRange _currRange;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
void ArithmeticDecoder::decode(
        auto& source,
        auto& dict,
        auto outIter,
        std::size_t wordsCount,
        std::size_t bitsLimit,
        std::optional<std::reference_wrapper<std::ostream>> os
) requires std::output_iterator<decltype(outIter), WordT> {
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

    auto barOpt = std::optional<boost::timer::progress_display>();
    if (os.has_value()) {
        barOpt.emplace(wordsCount, os.value(), "");
    }

    for (auto i : boost::irange<std::size_t>(0, wordsCount)) {
        const auto range128 = bm::uint128_t(_currRange.high - _currRange.low);
        const auto dictTotalWords128 = bm::uint128_t(dict.getTotalWordsCnt());
        const auto offset128 = bm::uint128_t(value - _currRange.low + 1);

        const auto aux128 = (offset128 * dictTotalWords128 - 1) / range128;
        const auto aux = aux128.convert_to<std::uint64_t>();
        const auto ord = dict.getWordOrd(aux);
        *outIter = WordT::byOrd(ord);
        ++outIter;

        auto [low, high, total] = dict.getProbabilityStats(ord);
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
        if (barOpt.has_value()) {
            ++barOpt.value();
        }
    }
}

}  // namespace ga

#endif // ARITHMETIC_DECODER_HPP

