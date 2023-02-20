#ifndef ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP
#define ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP

#include <unordered_map>
#include <cstdint>

#include "word_probability_stats.hpp"
#include "adaptive_d_dictionary.hpp"

#include <stdexcept>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
// \brief the PPMD dictionary class
class AdaptiveDContextualDictionary : protected AdaptiveDDictionary {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
private:

    using DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

    using _DDict = AdaptiveDDictionary;

    constexpr static std::uint8_t maxContextLength = 8;

public:

    /**
     * @brief D contextual dictionary constructor
     * @param wordNumBits - word bits length.
     * @param contextLength - number of context cells.
     * @param contextCellBitsLength - context cell bits length.
     */
    AdaptiveDContextualDictionary(std::uint16_t wordNumBits,
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

#endif // ADAPTIVE_D_CONTECTUAL_DICTIONARY_HPP
