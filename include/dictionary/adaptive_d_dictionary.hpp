#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"

#include <unordered_set>
#include <cstdint>

#include <boost/icl/interval_map.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class AdaptiveDDictionary {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<Count>;
public:
    AdaptiveDDictionary();
    AdaptiveDDictionary(AdaptiveDDictionary<WordT, CountT>&& other) = default;

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
    [[nodiscard]] ProbabilityStats getWordProbabilityStats(const WordT& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    Count totalWordsCount() const;

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    void _increaseWordCount(Ord ord);

public:
    boost::icl::interval_map<Ord, Count> _cumulativeFoundWordsCount;
    Count _totalFoundWordsCount;
    boost::icl::interval_map<Ord, Count> _cumulativeFoundUniueWords;
    Count _totalUniqueWords;
    std::unordered_map<Ord, Count> _foundWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveDDictionary<WordT, CountT>::AdaptiveDDictionary() : _totalFoundWordsCount(0), _totalUniqueWords(0) {}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
AdaptiveDDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
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
AdaptiveDDictionary<WordT, CountT>::getWordProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    auto wordCount = _foundWordsCount[ord];
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = low;
    if (_totalFoundWordsCount == 0) {
        ++high;
    } else {
        high += (Word::wordsCount - _foundWordsCount.size()) * 2 * wordCount + _totalUniqueWords - Word::wordsCount * wordCount;
    }
    auto total = totalWordsCount();
    _increaseWordCount(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::totalWordsCount() const -> Count {
    if (_totalFoundWordsCount == 0) {
        return WordT::wordsCount;
    }
    return 2 * (WordT::wordsCount - _foundWordsCount.size())
            * _totalFoundWordsCount;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void
AdaptiveDDictionary<WordT, CountT>::_increaseWordCount(Ord ord) {
    auto interval = boost::icl::interval<Ord>::right_open(ord, WordT::wordsCount);
    _cumulativeFoundWordsCount += std::make_pair(interval, Count{1});
    if (!_foundWordsCount.contains(ord)) {
        _cumulativeFoundUniueWords += std::make_pair(interval, Count{1});
    }
    ++_foundWordsCount[ord];
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (_totalFoundWordsCount == 0) {
        ord;
    }
    auto cumulativeUnique = _cumulativeFoundUniueWords(ord);
    return (WordT::wordsCount - cumulativeUnique) * 2 * _cumulativeFoundWordsCount(ord)
            + ord * _foundWordsCount.size() - WordT::wordsCount * cumulativeUnique;
}


}

#endif // ADAPTIVE_D_DICTIONARY_HPP
