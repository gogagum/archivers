#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"
#include "impl/a_d_dictionary_base.hpp"

#include <unordered_set>
#include <cstdint>
#include <boost/range/iterator_range.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDDictionary class
///
template <class WordT>
class AdaptiveDDictionary :
        protected impl::ADDictionaryBase<typename WordT::Ord, std::uint64_t, WordT::wordsCount> {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
public:
    AdaptiveDDictionary() = default;
    AdaptiveDDictionary(AdaptiveDDictionary<WordT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] WordT getWord(Count cumulativeNumFound) const;

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
    [[nodiscard]] Count getTotalWordsCnt() const;

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    Count _getWordCnt(Ord ord) const;

    ProbabilityStats _getProbabilityStats(Ord ord) const;

private:
    template <class _WordT>
    friend class PPMDDictionary;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
WordT
AdaptiveDDictionary<WordT>::getWord(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return WordT::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT>
auto AdaptiveDDictionary<WordT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    const auto ord = Word::ord(word);
    auto ret = _getProbabilityStats(ord);
    this->_updateWordCnt(ord, 1);
    return ret;
}

//----------------------------------------------------------------------------//
template <class WordT>
auto AdaptiveDDictionary<WordT>::getTotalWordsCnt() const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return WordT::wordsCount;
    }
    if (this->_getTotalUniqueWordsCnt() == WordT::wordsCount) {
        return this->_totalFoundWordsCnt;
    }
    return 2 * (WordT::wordsCount - this->_getTotalUniqueWordsCnt())
            * this->_totalFoundWordsCnt;
}

//----------------------------------------------------------------------------//
template <class WordT>
auto AdaptiveDDictionary<WordT>::_getLowerCumulativeCnt(Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return ord;
    }
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    const auto cumulativeWordsCnt = this->_cumulativeFoundWordsCnt.get(ord - 1);
    if (totalUniqueWordsCnt == WordT::wordsCount) {
        return cumulativeWordsCnt;
    }
    const auto cumulativeUniqueWordsCnt = this->_cumulativeFoundUniqueWords.get(ord - 1);
    return (WordT::wordsCount - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt
            + ord * totalUniqueWordsCnt
            - WordT::wordsCount * cumulativeUniqueWordsCnt;
}

//----------------------------------------------------------------------------//
template <class WordT>
auto AdaptiveDDictionary<WordT>::_getWordCnt(Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return 1;
    }
    const auto totalUniqueWordsCount = this->_getTotalUniqueWordsCnt();
    if (totalUniqueWordsCount == WordT::wordsCount) {
        return this->_foundWordsCount.at(ord);
    }
    const auto realWordCount = this->_getRealWordCnt(ord);
    return (Word::wordsCount - totalUniqueWordsCount) * 2 * realWordCount
        + totalUniqueWordsCount
        - Word::wordsCount * ((realWordCount > 0) ? 1 : 0);
}

//----------------------------------------------------------------------------//
template <class WordT>
auto AdaptiveDDictionary<WordT>::_getProbabilityStats(
        Ord ord) const -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    return { low, high, total };
}

}

#endif // ADAPTIVE_D_DICTIONARY_HPP
