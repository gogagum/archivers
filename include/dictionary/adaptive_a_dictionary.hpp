#ifndef ADAPTIVE_A_DICTIONARY_HPP
#define ADAPTIVE_A_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"
#include "impl/a_d_dictionary_base.hpp"

#include <algorithm>
#include <unordered_set>
#include <cstdint>

#include <boost/range/iterator_range.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveADictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class AdaptiveADictionary
        : impl::ADDictionaryBase<typename WordT::Ord, CountT, WordT::wordsCount> {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;
public:
    AdaptiveADictionary() = default;
    AdaptiveADictionary(AdaptiveADictionary<WordT, CountT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Word getWord(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(const Word& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    Count _getWordCnt(Ord ord) const;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveADictionary<WordT, CountT>::getWord(
        Count cumulativeNumFound) const -> Word {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return Word::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveADictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    const auto ord = Word::ord(word);
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    this->_updateWordCnt(ord, 1);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveADictionary<WordT, CountT>::getTotalWordsCnt() const -> Count {
    const auto uniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    if (WordT::wordsCount == uniqueWordsCnt) {
        return this->_totalFoundWordsCnt;
    }
    return (WordT::wordsCount - uniqueWordsCnt)
            * (this->_totalFoundWordsCnt + 1);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveADictionary<WordT, CountT>::_getLowerCumulativeCnt(
        Ord ord) const -> Count {
    const auto cumulativeNumFound = this->_cumulativeFoundWordsCnt(ord - 1);
    if (WordT::wordsCount == this->_foundWordsCount.size()) {
        return cumulativeNumFound;
    }
    const auto numUniqueWordsTotal = this->_getTotalUniqueWordsCnt();
    const auto cumulativeUniqueWordsNumFound =
            this->_cumulativeFoundUniqueWords(ord - 1);
    return (WordT::wordsCount - numUniqueWordsTotal) * cumulativeNumFound
            + (ord - cumulativeUniqueWordsNumFound);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveADictionary<WordT, CountT>::_getWordCnt(Ord ord) const -> Count {
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    if (WordT::wordsCount == totalUniqueWordsCnt) {
        return this->_foundWordsCount.at(ord);
    }
    return this->_foundWordsCount.contains(ord)
            ? this->_foundWordsCount.at(ord)
              * (WordT::wordsCount - totalUniqueWordsCnt)
            : 1;
}

}

#endif // ADAPTIVE_A_DICTIONARY_HPP
