#ifndef DECREASING_COUNTS_DICTIONARY_HPP
#define DECREASING_COUNTS_DICTIONARY_HPP

#include "word_probability_stats.hpp"

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingCountDictionary class
///
template <typename CountT>
class DecreasingCountDictionary {
public:
    using Word = CountT;
    using Ord = CountT;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;
public:
    DecreasingCountDictionary(Count initialCount);

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return word.
     */
    Word getWord(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word - number to get stats for.
     * @return [low, high, total] counts.
     */
    ProbabilityStats getWordProbabilityStats(Word word);

    /**
     * @brief totalWordsCount - current number of words.
     * @return number of words. In fact, it is last decoded/encoded word.
     */
    Count totalWordsCount() const;

private:
    Count _currentCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <typename CountT>
DecreasingCountDictionary<CountT>::DecreasingCountDictionary(Count initialCount)
    : _currentCount(initialCount) {}

//----------------------------------------------------------------------------//
template <typename CountT>
auto DecreasingCountDictionary<CountT>::getWord(
        Count cumulativeNumFound) const -> Word {
    return cumulativeNumFound;
}

//----------------------------------------------------------------------------//
template <typename CountT>
auto DecreasingCountDictionary<CountT>::getWordProbabilityStats(
        Word word) -> ProbabilityStats {
    auto ret = ProbabilityStats { word, word + 1, _currentCount };
    _currentCount = word;
    return ret;
}

//----------------------------------------------------------------------------//
template <typename CountT>
auto DecreasingCountDictionary<CountT>::totalWordsCount() const -> Count {
    return _currentCount;
}

}




#endif // DECREASING_COUNTS_DICTIONARY_HPP

