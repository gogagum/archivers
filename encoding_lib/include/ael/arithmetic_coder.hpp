#pragma once

#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <iostream>
#include <cstdint>
#include <optional>
#include <boost/timer/progress_display.hpp>

#include <ael/byte_data_constructor.hpp>
#include <ael/impl/ranges_calc.hpp>

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
class ArithmeticCoder {
public:

    struct EncodeRet {
        std::size_t wordsCount;
        std::size_t bitsEncoded;
    };

public:

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    template <class DictT>
    static EncodeRet encode(auto ordFlow,
                            ByteDataConstructor& dataConstructor,
                            DictT& dict,
                            auto os = std::nullopt);
};

////////////////////////////////////////////////////////////////////////////////
template <class DictT>
auto ArithmeticCoder::encode(
        auto ordFlow,
        ByteDataConstructor& dataConstructor, 
        DictT& dict,
        auto os) -> EncodeRet {
    auto ret = EncodeRet();
    using RC = impl::RangesCalc<typename DictT::Count, DictT::countNumBits>;
    auto currRange = typename RC::Range{ 0, RC::total };

    std::size_t btf = 0;

    auto barOpt = std::optional<boost::timer::progress_display>();
    if (os.has_value()) {
        barOpt.emplace(ordFlow.size(), os.value(), "");
    }

    for (auto ord : ordFlow) {
        const auto [low, high, total] = dict.getProbabilityStats(ord);
        currRange = RC::rangeFromStatsAndPrev(currRange, low, high, total);

        while (true) {
            if (currRange.high <= RC::half) {
                ret.bitsEncoded += btf + 1;
                dataConstructor.putBit(false);
                dataConstructor.putBitsRepeatWithReset(true, btf);
            } else if (currRange.low >= RC::half) {
                ret.bitsEncoded += btf + 1;
                dataConstructor.putBit(true);
                dataConstructor.putBitsRepeatWithReset(false, btf);
            } else if (currRange.low >= RC::quater
                       && currRange.high <= RC::threeQuaters) {
                ++btf;
            } else {
                break;
            }
            currRange = RC::recalcRange(currRange);
        }
        ++ret.wordsCount;
        if (barOpt.has_value()) {
            ++barOpt.value();
        }
    }

    ret.bitsEncoded += btf + 2;
    if (currRange.low < RC::quater) {
        dataConstructor.putBit(false);
        dataConstructor.putBitsRepeat(true, btf + 1);
    } else {
        dataConstructor.putBit(true);
        dataConstructor.putBitsRepeat(false, btf + 1);
    }

    return ret;
}

}  // namespace ael

#endif // ARITHMETIC_CODER_HPP

