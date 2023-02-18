#ifndef PPMA_DICTIONARY_HPP
#define PPMA_DICTIONARY_HPP

#include <cstdint>
#include <unordered_map>
#include "adaptive_a_dictionary.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
// \brief The PPMA Dictionary class
class PPMADictionary : protected AdaptiveADictionary {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
private:

    using DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

    using _DDict = AdaptiveADictionary;

    constexpr static std::uint8_t maxContextLength = 8;

public:

    /**
     * @brief PPMD dictionary constructor
     * @param wordNumBits - word bits length.
     * @param contextLength - number of context cells.
     * @param contextCellBitsLength - context cell bits length.
     */
    PPMADictionary(std::uint16_t wordNumBits,
                   std::uint16_t contextLength,
                   std::uint16_t contextCellBitsLength);

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return totalWordsCount according to dictionary model.
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

private:

    void _updateCtx(Ord ord);

private:

    struct _SearchCtx {
        std::uint16_t length;
        Ord ctx;
        friend bool operator==(_SearchCtx, _SearchCtx) = default;
    };

    struct _SearchCtxHash {
        std::size_t operator()(_SearchCtx searchCtx) const {
            return static_cast<std::size_t>(searchCtx.ctx)
                ^ std::size_t{searchCtx.length};
        }
    };

private:
    std::unordered_map<_SearchCtx, _DDict, _SearchCtxHash> _contextProbs;
    Ord _ctx;
    std::uint16_t _currCtxLength;
    const std::uint16_t _numBits;
    const std::uint16_t _ctxCellBitsLength;
    const std::uint16_t _ctxLength;
};

}

#endif  // PPMA_DICTIONARY_HPP