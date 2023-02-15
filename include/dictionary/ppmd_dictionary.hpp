#ifndef PPMD_DICTIONARY_HPP
#define PPMD_DICTIONARY_HPP

#include <unordered_map>
#include <deque>
#include <cstdint>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>

#include <dictionary/word_probability_stats.hpp>
#include <dictionary/adaptive_d_dictionary.hpp>

#include <stdexcept>

namespace ga::dict {

class PPMDDictionary : protected AdaptiveDDictionary {
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

    PPMDDictionary(std::uint16_t numBits,
                   std::uint16_t contextLength,
                   std::uint16_t contextCellBitsLength)
        : AdaptiveDDictionary(1ull << numBits),
          _ctx(0),
          _currCtxLength(0),
          _numBits(numBits),
          _ctxCellBitsLength(contextCellBitsLength),
          _ctxLength(contextLength) {
        if (contextCellBitsLength * contextLength > 56) {
            throw std::invalid_argument("Too big context length.");
        }
    }

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
            return std::size_t{searchCtx.ctx} ^ std::size_t{searchCtx.length};
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

#endif // PPMD_DICTIONARY_HPP
