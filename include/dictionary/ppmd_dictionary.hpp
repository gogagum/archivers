#ifndef PPMD_DICTIONARY_HPP
#define PPMD_DICTIONARY_HPP

#include <unordered_set>
#include <deque>
#include <cassert>
#include <cstdint>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>

#include <dst/dynamic_segment_tree.hpp>
#include <dictionary/word_probability_stats.hpp>
#include <dictionary/adaptive_d_dictionary.hpp>

namespace ga::dict {

template <class WordT>
class PPMDDictionary : protected AdaptiveDDictionary<WordT> {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
private:

    using DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

    using _DDict = AdaptiveDDictionary<Word>;

    constexpr static std::uint8_t maxContextLength = 8;
    using _Ctx = std::deque<Ord>;
    using _SearchCtx = boost::container::static_vector<Ord, maxContextLength>;
    using _SearchCtxHash = boost::hash<_SearchCtx>;

public:

    PPMDDictionary(std::uint8_t contextLen = 5) : _contextLen(contextLen) {
        assert(contextLen < maxContextLength && "Unsupported context length.");
    }

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return
     */
    [[nodiscard]] Word getWord(Count cumulativeNumFound) const {
        for (auto ctx = _getInitSearchCtx(); !ctx.empty(); ctx.pop_back()) {
            if (_contextProbs.contains(ctx)) {
                return _contextProbs.at(ctx).getWord(cumulativeNumFound);
            }
        }
        return AdaptiveDDictionary<WordT>::getWord(cumulativeNumFound);
    }

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(const Word& word) {
        const auto ord = Word::ord(word);
        std::optional<ProbabilityStats> ret;

        for (auto ctx = _getInitSearchCtx(); !ctx.empty(); ctx.pop_back()) {
            if (_contextProbs.contains(ctx) && !ret.has_value()) {
                ret = _contextProbs.at(ctx)._getProbabilityStats(ord);
            }
            _contextProbs[ctx]._updateWordCnt(ord, 1);
        }
        ret = ret.value_or(this->_getProbabilityStats(ord));
        this->_updateWordCnt(ord, 1);
        _updateCtx(ord);
        return ret.value();
    }

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return totalWordsCount according to dictionary model.
     */
    [[nodiscard]] Count getTotalWordsCnt() const {
        for (auto ctx = _getInitSearchCtx(); !ctx.empty(); ctx.pop_back()) {
            if (_contextProbs.contains(ctx)) {
                return _contextProbs.at(ctx).getTotalWordsCnt();
            }
        }
        return AdaptiveDDictionary<WordT>::getTotalWordsCnt();
    }

private:

    _SearchCtx _getInitSearchCtx() const { return {_ctx.rbegin(), _ctx.rend()}; }

    void _updateCtx(typename Word::Ord ord) {
        if (_ctx.size() == _contextLen) {
            _ctx.pop_front();
        }
        _ctx.push_back(ord);
    }

private:
    std::unordered_map<_SearchCtx, _DDict, _SearchCtxHash> _contextProbs;
    _Ctx _ctx;
    const std::uint8_t _contextLen;
};

}

#endif // PPMD_DICTIONARY_HPP
