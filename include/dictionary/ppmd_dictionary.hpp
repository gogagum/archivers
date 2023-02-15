#ifndef PPMD_DICTIONARY_HPP
#define PPMD_DICTIONARY_HPP

#include <unordered_map>
#include <deque>
#include <cstdint>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>

#include <dictionary/word_probability_stats.hpp>
#include <dictionary/adaptive_d_dictionary.hpp>

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
    using _Ctx = std::deque<Ord>;
    using _SearchCtx = boost::container::static_vector<Ord, maxContextLength>;
    using _SearchCtxHash = boost::hash<_SearchCtx>;

public:

    PPMDDictionary(Ord maxOrd, std::uint8_t contextLen = 5)
        : AdaptiveDDictionary(maxOrd),
          _contextLen(contextLen) {
        assert(contextLen < maxContextLength && "Unsupported context length.");
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

    _SearchCtx _getInitSearchCtx() const { return {_ctx.rbegin(), _ctx.rend()}; }

    void _updateCtx(Ord ord);

private:
    std::unordered_map<_SearchCtx, _DDict, _SearchCtxHash> _contextProbs;
    _Ctx _ctx;
    const std::uint8_t _contextLen;
};

}

#endif // PPMD_DICTIONARY_HPP
