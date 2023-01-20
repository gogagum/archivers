#ifndef DECREASING_COUNTS_DICTIONARY_HPP
#define DECREASING_COUNTS_DICTIONARY_HPP

#include <stdexcept>

#include "word_probability_stats.hpp"
#include "word/uint_word.hpp"

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingCountDictionary class
///
template <typename CountT>
class DecreasingCountDictionary {
public:
    using Word = w::UIntWord<CountT>;
    using Ord = CountT;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The NonDecreasingWord class
    ///
    class NonDecreasingWord : public std::logic_error {
    public:
        NonDecreasingWord() : std::logic_error("Count increased.") {};
    };

public:
    DecreasingCountDictionary(Count initialCount);

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return word.
     */
    [[nodiscard]] Word getWord(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word - number to get stats for.
     * @return [low, high, total] counts.
     */
    [[nodiscard]] ProbabilityStats getWordProbabilityStats(Word word);

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
    if (cumulativeNumFound > _currentCount) {
        throw NonDecreasingWord();
    }
    return Word(cumulativeNumFound);
}

//----------------------------------------------------------------------------//
template <typename CountT>
auto DecreasingCountDictionary<CountT>::getWordProbabilityStats(
        Word word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    if (ord > _currentCount) {
        throw NonDecreasingWord();
    }
    auto ret = ProbabilityStats{ ord, ord + 1, _currentCount };
    _currentCount = ord;
    return ret;
}

//----------------------------------------------------------------------------//
template <typename CountT>
auto DecreasingCountDictionary<CountT>::totalWordsCount() const -> Count {
    return _currentCount;
}

}




#endif // DECREASING_COUNTS_DICTIONARY_HPP

