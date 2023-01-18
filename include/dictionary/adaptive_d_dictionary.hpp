#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"

#include <unordered_set>
#include <cstdint>

#include <boost/icl/interval_map.hpp>

namespace ga::dict {

template <class WordT, typename CountT = std::uint64_t>
class AdaptiveDDictionary {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
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
     * @brief getLowerCumulativeNumFound - lower letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    Count getLowerCumulativeNumFound(const WordT& word) const;

    /**
     * @brief getHigherCumulativeNumFound - lower or equal letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    Count getHigherCumulativeNumFound(const WordT& word) const;

    /**
     * @brief totalWordsCount
     * @return
     */
    Count totalWordsCount() const;

    /**
     * @brief increaseWordCount
     * @param word - word which count we increase.
     */
    void increaseWordCount(const WordT& word);

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    Count _getLowerUnique(Ord ord) const;

public:
    boost::icl::interval_map<Ord, Count> _cumulativeFoundWordsCount;
    Count _totalFoundWordsCount;
    boost::icl::interval_map<Ord, Count> _cumulativeFoundUniueWords;
    std::unordered_map<Ord, Count> _foundWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveDDictionary<WordT, CountT>::AdaptiveDDictionary() {}

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
AdaptiveDDictionary<WordT, CountT>::getLowerCumulativeNumFound(
        const WordT& word) const -> Count {
    return _getLowerCumulativeNumFound(WordT::ord(word));
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDDictionary<WordT, CountT>::getHigherCumulativeNumFound(
        const WordT& word) const -> Count {
    return _getLowerCumulativeNumFound(WordT::ord(word) + 1);
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
AdaptiveDDictionary<WordT, CountT>::increaseWordCount(const WordT& word) {
    auto interval =
        boost::icl::interval<Ord>::right_open(WordT::ord(word), WordT::wordsCount);
    _cumulativeFoundWordsCount += std::make_pair(interval, 1);
    if (!_foundWordsCount.contains(WordT::ord(word))) {
        _cumulativeFoundUniueWords += std::make_pair(interval, 1);
        _foundWordsCount.emplace(word, 1);
    } else {
        ++_foundWordsCount.at(word);
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (_totalFoundWordsCount == 0) {
        ord - 1;
    }
    auto cumulativeUnique = _cumulativeFoundUniueWords(ord)
    return (WordT::wordsCount - cumulativeUnique) * 2 * _cumulativeFoundWordsCount(ord)
            + ord * _foundWordsCount.size() - WordT::wordsCount * cumulativeUnique;
}


}

#endif // ADAPTIVE_D_DICTIONARY_HPP
