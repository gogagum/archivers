#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/range/irange.hpp>
#include <boost/timer/progress_display.hpp>

#include <iostream>
#include <cstdint>
#include <limits>
#include <optional>

#include "impl/ranges_calc.hpp"
#include "impl/multiply_and_divide.hpp"

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
class ArithmeticDecoder {
public:
    template <std::output_iterator<std::uint64_t> OutIter, class Dict>
    static void decode(
            auto& source,
            Dict& dict,
            OutIter outIter,
            std::size_t wordsCount,
            std::size_t bitsLimit = std::numeric_limits<std::size_t>::max(),
            auto&& tick = [](){}
        );
};

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter, class Dict>
void ArithmeticDecoder::decode(
        auto& source,
        Dict& dict,
        OutIter outIter,
        std::size_t wordsCount,
        std::size_t bitsLimit,
        auto&& tick) {
    const auto takeBitLimited = [&source, &bitsLimit]() -> bool {
        if (bitsLimit == 0) {
            return false;
        }
        --bitsLimit;
        return source.takeBit();
    };

    using RC = impl::RangesCalc<typename Dict::Count, Dict::countNumBits>;
    typename RC::Range currRange;
    typename RC::Count value = 0;

    for (auto _ : boost::irange<std::size_t>(0, Dict::countNumBits)) {
        value = (value << 1) + (takeBitLimited() ? 1 : 0);
    }

    auto barOpt = std::optional<boost::timer::progress_display>();

    for (auto i : boost::irange<std::size_t>(0, wordsCount)) {
        const auto range =
            typename Dict::Count{currRange.high - currRange.low};
        const auto dictTotalWords = dict.getTotalWordsCnt();
        const auto offset = value - currRange.low + 1;
        assert(offset < range);
        const auto aux =
            impl::multiply_decrease_and_divide(offset, dictTotalWords, range);
        const auto ord = dict.getWordOrd(aux);
        *outIter = ord;
        ++outIter;

        auto [low, high, total] = dict.getProbabilityStats(ord);
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
        if (barOpt.has_value()) {
            ++barOpt.value();
        }
        tick();
    }
}

}  // namespace ael

#endif // ARITHMETIC_DECODER_HPP

