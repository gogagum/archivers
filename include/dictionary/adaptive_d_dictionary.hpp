#ifndef ADAPTIVE_D_DICTIONARY_HPP
#define ADAPTIVE_D_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"
#include "impl/a_d_dictionary_base.hpp"

#include <unordered_set>
#include <cstdint>

namespace ga::dict {

namespace bicl = boost::icl;

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class AdaptiveDDictionary :
        impl::ADDictionaryBase<typename WordT::Ord, CountT, WordT::wordsCount> {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<Count>;
public:
    AdaptiveDDictionary() = default;
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
    Count getTotalWordsCnt() const;

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    Count _getWordCnt(Ord ord) const;
};

////////////////////////////////////////////////////////////////////////////////
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
auto AdaptiveDDictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    const auto ord = Word::ord(word);
    const auto low = _getLowerCumulativeNumFound(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    this->_updateWordCnt(ord, 1);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::getTotalWordsCnt() const -> Count {
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
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return ord;
    }
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    const auto cumulativeWordsCnt = this->_cumulativeFoundWordsCnt(ord - 1);
    if (totalUniqueWordsCnt == WordT::wordsCount) {
        return cumulativeWordsCnt;
    }
    const auto cumulativeUniqueWordsCnt = this->_cumulativeFoundUniqueWords(ord - 1);
    return (WordT::wordsCount - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt
            + ord * totalUniqueWordsCnt
            - WordT::wordsCount * cumulativeUniqueWordsCnt;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDDictionary<WordT, CountT>::_getWordCnt(Ord ord) const -> Count {
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

}

#endif // ADAPTIVE_D_DICTIONARY_HPP
