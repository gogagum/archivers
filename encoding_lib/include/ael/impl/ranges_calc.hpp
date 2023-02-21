#ifndef RANGES_CALC_HPP
#define RANGES_CALC_HPP

#include <cstdint>
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

namespace ael::impl {

namespace bm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangesCalc class
///
class RangesCalc {
public:

    using Count = std::uint64_t;
    struct Range;

public:

    constexpr static const std::uint16_t numBits = 56;
    constexpr static const Count total = Count{1} << Count{numBits};
    constexpr static const Count half = Count{1} << Count{numBits - 1};
    constexpr static const Count quater = Count{1} << Count{numBits - 2};
    constexpr static const Count threeQuaters = 3 * quater;

public:

    static Range recalcRange(Range r);

    static Range rangeFromStatsAndPrev(
            Range r, Count low, Count high, Count total);

};

////////////////////////////////////////////////////////////////////////////////
/// \brief The Range class
///
struct RangesCalc::Range {
    Count low;
    Count high;
};

}

#endif // RANGES_CALC_HPP
