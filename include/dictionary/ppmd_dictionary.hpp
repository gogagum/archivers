#ifndef PPMD_DICTIONARY_HPP
#define PPMD_DICTIONARY_HPP

#include <unordered_set>
#include <deque>
#include <cstdint>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>

#include <dst/dynamic_segment_tree.hpp>
#include <dictionary/impl/a_d_dictionary_base.hpp>
#include <dictionary/word_probability_stats.hpp>
#include <dictionary/adaptive_d_dictionary.hpp>

namespace ga::dict {

template <class WordT, std::uint8_t contextLen = 5, typename CountT = std::uint64_t>
class PPMDDictionary
        : impl::ADDictionaryBase<typename WordT::Ord, CountT, WordT::wordsCount>  {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;
private:

    using DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;


    using СonditionalProbability = impl::ADDictionaryBase<typename WordT::Ord, CountT, WordT::wordsCount>;

    using Context = std::deque<Ord>;
    using SearchContext = boost::container::static_vector<Ord, contextLen>;

public:

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return
     */
    [[nodiscard]] Word getWord(Count cumulativeNumFound) const {
        auto currCtx = SearchContext(_ctx.rbegin(), _ctx.rend());
        do {
            if (_foundContext(currCtx)) {
                return _contextProbs.at(currCtx).getWord(cumulativeNumFound);
            }
            currCtx.pop_back();
        } while (!currCtx.empty());
    }

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(const Word& word) {
        const auto ord = Word::ord(word);
        std::optional<ProbabilityStats> ret;

        {
            auto currCtx = SearchContext(_ctx.rbegin(), _ctx.rend());
            do {
                if (_contextProbs.contains(currCtx)) {
                    ret = _contextProbs.at(currCtx)._getProbabilityStats(ord);
                    break;
                }
                currCtx.pop_back();
            } while (!currCtx.empty());
        }

        {
            auto currCtx = SearchContext(_ctx.rbegin(), _ctx.rend());
            do {
                _contextProbs[currCtx]._updateWordCnt(ord, 1);
                currCtx.pop_back();
            } while (!currCtx.empty());

            _ctx.pop_front();
            _ctx.push_back(ord);
        }

        return ret.value();
    }

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return totalWordsCount according to dictionary model.
     */
    [[nodiscard]] Count getTotalWordsCnt() const {
        auto currCtx = SearchContext(_ctx.rbegin(), _ctx.rend());
        do {
            if (_contextProbs.contains(currCtx)) {
                return _contextProbs.at(currCtx).getTotalWordsCnt();
            }
        } while (!currCtx.empty());
    }

private:
    std::unordered_map<SearchContext, AdaptiveDDictionary<WordT, CountT>, boost::hash<SearchContext>> _contextProbs;
    Context _ctx;
};

}

#endif // PPMD_DICTIONARY_HPP
