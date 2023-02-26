#ifndef CUMULATIVE_COUNT_HPP
#define CUMULATIVE_COUNT_HPP

#include <cstdint>
#include <dst/dynamic_segment_tree.hpp>
#include <unordered_map>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
// 
class CumulativeCount {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
public:

    /**
     * @brief CumulativeCount constructor.
     * @param maxOrd - maximalOrder;
     */
    CumulativeCount(Ord maxOrd);

    /**
     * @brief increaseOrdCount - increase one word count.
     * @param ord - where to increase count.
     */
    void increaseOrdCount(Ord ord, std::int64_t cntChange);

    /**
     * @brief getCumulativeCount - get cumulative count from zero
     * to given ord including it.
     * @param ord - order index of a checked word.
     * @return cumulative count.
     */
    Count getCumulativeCount(Ord ord) const;

    /**
     * @brief get count of a word.
     * @param ord - order index of a word.
     * @return word count.
     */
    Count getCount(Ord ord) const;

    /**
     * @brief get total words count.
     * @return total words count.
     */
    Count getTotalWordsCnt() const;

private:
    using _DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;
private:
    _DST _cumulativeCnt;
    std::unordered_map<Ord, Count> _cnt;
    Count _totalWordsCnt;
    const Ord _maxOrd;
};

}  // namespace ael::dict::impl

#endif  // CUMULATIVE_COUNT_HPP
