#ifndef RANGES_CALC_HPP
#define RANGES_CALC_HPP

#include <cstdint>

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangesCalc class
///
template <class SymT>
class RangesCalc {
public:

    using Count = std::uint64_t;
    constexpr static Count _computeCorrectingConst();

public:

    constexpr static Count symsNum = SymT::wordsCount;
    constexpr static Count symsNum_2 = symsNum / Count{2};
    constexpr static Count symsNum_4 = symsNum / Count{4};
    constexpr static Count threeSymsNum = Count{3} * symsNum;
    constexpr static Count symsNum_3to4 = threeSymsNum / Count{4};
    constexpr static Count correctingConst = _computeCorrectingConst();
    constexpr static Count correctedSymsNum = symsNum * correctingConst;
    constexpr static Count correctedSymsNum_2 = symsNum_2 * correctingConst;
    constexpr static Count correctedSymsNum_4 = symsNum_4 * correctingConst;
    constexpr static Count correctedSymsNum_3to4 = symsNum_3to4 * correctingConst;

public:

    struct Range {
        Count low;
        Count high;
    };

public:

    static Range recalcRange(Range r);

public:

    constexpr static std::uint16_t _computeAdditionalBitsCnt();

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT>
constexpr auto RangesCalc<SymT>::_computeCorrectingConst() -> Count {
    auto correctingConst = Count{1};
    while (correctingConst * symsNum < (Count{1} << Count{33})) {
        correctingConst <<= 1ull;
    }
    return correctingConst;
}


//----------------------------------------------------------------------------//
template <class SymT>
auto RangesCalc<SymT>::recalcRange(Range r) -> Range {
    if (r.high <= symsNum_2 * correctingConst) {
        return { r.low * 2, r.high * 2 };
    } else if (r.low >= symsNum_2 * correctingConst) {
        return {
            r.low * 2 - symsNum * correctingConst,
            r.high * 2 - symsNum * correctingConst
        };
    } else if (r.low >= symsNum_4 * correctingConst
               && r.high <= symsNum_3to4 * correctingConst) {
        return {
            r.low * 2 - symsNum_2 * correctingConst,
            r.high * 2 - symsNum_2 * correctingConst
        };
    }
    return r;
}

//----------------------------------------------------------------------------//
template <class SymT>
constexpr std::uint16_t RangesCalc<SymT>::_computeAdditionalBitsCnt() {
    std::uint16_t ret = 0;
    for (; (symsNum << ret) < (1ull << 33); ++ret) {}
    return ret;
}

}

#endif // RANGES_CALC_HPP
