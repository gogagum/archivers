#ifndef ADAPTIVE_A_DICTIONARY_HPP
#define ADAPTIVE_A_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"

#include <unordered_set>
#include <cstdint>

#include <boost/icl/interval_map.hpp>

namespace ga::dict {

namespace bicl = boost::icl;

template <class WordT, typename CountT = std::uint64_t>
class AdaptiveADictionary {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;
public:
    AdaptiveADictionary();
    AdaptiveADictionary(AdaptiveADictionary<WordT, CountT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    WordT getWord(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    ProbabilityStats getWordProbabilityStats(const WordT& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    Count totalWordsCount() const;

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    Count _getLowerUnique(Ord ord) const;

    void _increaseWordCount(Ord word);

public:
    bicl::interval_map<Ord, Count> _cumulativeFoundWordsCount;
    Count _totalFoundWordsCount;
    bicl::interval_map<Ord, Count> _cumulativeFoundUniueWords;
    std::unordered_map<Ord, Count> _foundWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveADictionary<WordT, CountT>::AdaptiveADictionary() {}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
AdaptiveADictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        return this->_additionalCounts(ord) + ord + 1;
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return WordT::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveADictionary<WordT, CountT>::getWordProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = _foundWordsCount.contains(ord) ? _foundWordsCount.at(ord) : 1;
    auto total = totalWordsCount();
    _increaseWordCount(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveADictionary<WordT, CountT>::totalWordsCount() const -> Count {
    return (WordT::wordsCount - _foundWordsCount.size())
            * (_totalFoundWordsCount + 1);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void
AdaptiveADictionary<WordT, CountT>::_increaseWordCount(Ord ord) {
    auto interval =
        bicl::interval<Ord>::right_open(ord, WordT::wordsCount);
    _cumulativeFoundWordsCount += std::make_pair(interval, Count{1});
    if (!_foundWordsCount.contains(ord)) {
        _cumulativeFoundUniueWords += std::make_pair(interval, Count{1});
        _foundWordsCount.insert({ord, 1});
    } else {
        ++_foundWordsCount.at(ord);
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveADictionary<WordT, CountT>::_getLowerCumulativeNumFound(Ord ord) const -> Count {
    if (ord == 0) {
        return (WordT::wordsCount - _foundWordsCount.size());
    }
    return (WordT::wordsCount - _foundWordsCount.size()) * _cumulativeFoundWordsCount(ord - 1)
            + (WordT::wordsCount - _cumulativeFoundUniueWords(ord - 1));
}


}

#endif // ADAPTIVE_A_DICTIONARY_HPP
