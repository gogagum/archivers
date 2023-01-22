#ifndef STATIC_DICTIONARY_HPP
#define STATIC_DICTIONARY_HPP

#include "word_probability_stats.hpp"

#include <vector>
#include <boost/range/irange.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
template <class WordT, class CountT = std::uint64_t>
class StaticDictionary {
public:

    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;

private:

    StaticDictionary() = default;

public:

    StaticDictionary(const StaticDictionary& other) = default;
    StaticDictionary(StaticDictionary&& other) = default;

    /**
     * @brief fromCounts construct from simple wordsCounts
     * @param countsRng - count of each symbol ordered.
     * @return
     */
    template <class RangeT>
    StaticDictionary(const RangeT& countsRng);

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    WordT getWord(Count cumulativeNumFound) const;

    /**
     * @brief getProbabilityStats - get lower cumulative number of words,
     * higher cumulative number of words and total count.
     * @param word - word to get info for.
     * @return statistics.
     */
    ProbabilityStats getProbabilityStats(const Word& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    Ord getTotalWordsCount() const;

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;
    Count _getHigherCumulativeNumFound(Ord ord) const;

protected:
    std::vector<Count> _cumulativeNumFound;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class RangeT>
StaticDictionary<WordT, CountT>::StaticDictionary(const RangeT& countMap) {
    _cumulativeNumFound.resize(Word::wordsCount);
    auto currOrd = Ord{0};
    auto currCumulativeNumFound = Count{0};
    for (auto& [word, count]: countMap) {
        auto ord = WordT::ord(word);
        for (; currOrd < ord; ++currOrd) {
            _cumulativeNumFound[currOrd] = currCumulativeNumFound;
        }
        currCumulativeNumFound += count;
    }
    for (; currOrd < Word::wordsCount; ++currOrd) {
        _cumulativeNumFound[currOrd] = currCumulativeNumFound;
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
StaticDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
    auto it = std::ranges::upper_bound(_cumulativeNumFound, cumulativeNumFound);
    return WordT::byOrd(it - _cumulativeNumFound.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
StaticDictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = _getHigherCumulativeNumFound(ord);
    return { low, high, *_cumulativeNumFound.rbegin() };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
StaticDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (ord == 0) {
        return 0;
    } else {
        return _cumulativeNumFound[ord - 1];
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
StaticDictionary<WordT, CountT>::_getHigherCumulativeNumFound(
        Ord ord) const -> Count {
    return _cumulativeNumFound[ord];
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto StaticDictionary<WordT, CountT>::getTotalWordsCount() const -> Ord {
    return *_cumulativeNumFound.rbegin();
}

}  // namespace ga::dict

#endif // STATIC_DICTIONARY_HPP
