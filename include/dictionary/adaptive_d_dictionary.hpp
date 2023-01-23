#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"

#include <unordered_set>
#include <cstdint>

#include <boost/icl/interval_map.hpp>

namespace ga::dict {

namespace bicl = boost::icl;

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
    [[nodiscard]] ProbabilityStats getProbabilityStats(const WordT& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    Count getTotalWordsCount() const;

private:

    Count _getLowerCumulativeFoundUniueWords(Ord ord) const;

    Count _getWordCount(Ord ord) const;

    Count _getLowerCumulativeNumFound(Ord ord) const;

    void _increaseWordCount(Ord ord);

private:

    using OrdInterval = typename bicl::interval_map<Ord, Count>::interval_type;

private:
    boost::icl::interval_map<Ord, Count> _cumulativeFoundWordsCount;
    Count _totalFoundWordsCount;
    boost::icl::interval_map<Ord, Count> _cumulativeFoundUniueWords;
    Count _totalUniqueWords;
    std::unordered_map<Ord, Count> _foundWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveDDictionary<WordT, CountT>::AdaptiveDDictionary()
    : _totalFoundWordsCount(0), _totalUniqueWords(0) {}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
AdaptiveDDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeNumFound(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return WordT::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDDictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = low + _getWordCount(ord);
    auto total = getTotalWordsCount();
    _increaseWordCount(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::getTotalWordsCount() const -> Count {
    if (_totalFoundWordsCount == 0) {
        return WordT::wordsCount;
    }
    if (_totalUniqueWords == WordT::wordsCount) {
        return _totalFoundWordsCount;
    }
    return 2 * (WordT::wordsCount - _totalUniqueWords)
            * _totalFoundWordsCount;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void
AdaptiveDDictionary<WordT, CountT>::_increaseWordCount(Ord ord) {
    auto interval = OrdInterval(ord, WordT::wordsCount);
    _cumulativeFoundWordsCount += std::make_pair(interval, Count{1});
    if (!_foundWordsCount.contains(ord)) {
        _cumulativeFoundUniueWords += std::make_pair(interval, Count{1});
        ++_totalUniqueWords;
    }
    ++_totalFoundWordsCount;
    ++_foundWordsCount[ord];
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDDictionary<WordT, CountT>::_getLowerCumulativeFoundUniueWords(
        Ord ord) const -> Count {
    return _cumulativeFoundUniueWords(ord - 1);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (_totalFoundWordsCount == 0) {
        return ord;
    }
    if (_totalUniqueWords == WordT::wordsCount) {
        return _cumulativeFoundWordsCount(ord - 1);
    }
    auto cumulativeUnique = _cumulativeFoundUniueWords(ord - 1);
    return (WordT::wordsCount - _totalUniqueWords) * 2 * _cumulativeFoundWordsCount(ord - 1)
            + ord * _foundWordsCount.size() - WordT::wordsCount * cumulativeUnique;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::_getWordCount(Ord ord) const -> Count {
    if (_totalFoundWordsCount == 0) {
        return 1;
    }
    if (_totalUniqueWords == WordT::wordsCount) {
        return _foundWordsCount.at(ord);
    }
    auto realWordCount =
        _foundWordsCount.contains(ord) ? _foundWordsCount.at(ord) : 0;
    return (Word::wordsCount - _totalUniqueWords) * 2 * realWordCount
        + _totalUniqueWords - Word::wordsCount * ((realWordCount > 0) ? 1 : 0);
}


}

#endif // ADAPTIVE_D_DICTIONARY_HPP
