#ifndef RANGES_CALC_HPP
#define RANGES_CALC_HPP

#include <cstdint>

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The RangesCalc class
///
template <class SymT>
class RangesCalc {
private:

    constexpr static std::uint64_t _computeCorrectingConst();

protected:

    constexpr static auto symsNum = static_cast<std::uint64_t>(SymT::wordsCount);
    constexpr static auto symsNum_2 = symsNum / 2;
    constexpr static auto symsNum_4 = symsNum / 4;
    constexpr static auto symsNum_3to4 = 3 * symsNum / 4;
    constexpr static auto correctingConst = _computeCorrectingConst();
    constexpr static auto correctedSymsNum = symsNum * correctingConst;
    constexpr static auto correctedSymsNum_2 = symsNum_2 * correctingConst;
    constexpr static auto correctedSymsNum_4 = symsNum_4 * correctingConst;
    constexpr static auto correctedSymsNum_3to4 = symsNum_3to4 * correctingConst;

public:

    struct Range {
        std::uint64_t low;
        std::uint64_t high;
    };

public:

    static Range recalcRange(Range r);

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT>
constexpr std::uint64_t RangesCalc<SymT>::_computeCorrectingConst() {
    std::uint64_t correctingConst = 1;
    while (correctingConst * symsNum < (1ull << 40)) {
        correctingConst <<= 1;
    }
    return correctingConst;
}


//----------------------------------------------------------------------------//
template <class SymT>
auto RangesCalc<SymT>::recalcRange(Range r) -> Range {
    Range ret;
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

}

#endif // RANGES_CALC_HPP
