#ifndef RANGES_CALC_HPP
#define RANGES_CALC_HPP

#include <cstdint>
#include <iostream>

namespace ga {

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

};

////////////////////////////////////////////////////////////////////////////////
/// \brief The Range class
///
struct RangesCalc::Range {
    Count low;
    Count high;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
auto RangesCalc::recalcRange(Range r) -> Range {
    if (r.high <= half) {
        return { r.low * 2, r.high * 2 };
    } else if (r.low >= half) {
        return { r.low * 2 - total, r.high * 2 - total };
    } else if (r.low >= quater && r.high <= threeQuaters) {
        return { r.low * 2 - half, r.high * 2 - half };
    }
    return r;
}

}

#endif // RANGES_CALC_HPP
