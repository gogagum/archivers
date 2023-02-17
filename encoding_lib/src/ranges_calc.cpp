#include <ael/impl/ranges_calc.hpp>

namespace ael::impl {
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

//----------------------------------------------------------------------------//
auto RangesCalc::rangeFromStatsAndPrev(
        Range r, Count low, Count high, Count total) -> Range {
    const auto range128 = bm::uint128_t(r.high - r.low);
    const auto low128 = bm::uint128_t(low);
    const auto high128 = bm::uint128_t(high);
    const auto total128 = bm::uint128_t(total);

    const auto lowScaled128 = (range128 * low128) / total128;
    const auto highScaled128 = ((range128 * high128) / total128);

    return {
        r.low + lowScaled128.convert_to<std::uint64_t>(),
        r.low + highScaled128.convert_to<std::uint64_t>()
    };
}

}