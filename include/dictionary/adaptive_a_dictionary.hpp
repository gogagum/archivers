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
template <std::integral OrdT = std::uint64_t>
class AdaptiveADictionary
        : impl::ADDictionaryBase<OrdT, std::uint64_t> {
public:
    using Ord = OrdT;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
public:
    AdaptiveADictionary(OrdT maxOrd) : impl::ADDictionaryBase<OrdT, std::uint64_t>(maxOrd) {}
    AdaptiveADictionary(AdaptiveADictionary<OrdT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] OrdT getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

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
template <std::integral OrdT>
auto AdaptiveADictionary<OrdT>::getWordOrd(
        Count cumulativeNumFound) const -> OrdT {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveADictionary<OrdT>::getProbabilityStats(Ord ord) -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    this->_updateWordCnt(ord, 1);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveADictionary<OrdT>::getTotalWordsCnt() const -> Count {
    const auto uniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    if (this->_maxOrd == uniqueWordsCnt) {
        return this->_totalFoundWordsCnt;
    }
    return (this->_maxOrd - uniqueWordsCnt)
            * (this->_totalFoundWordsCnt + 1);
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveADictionary<OrdT>::_getLowerCumulativeCnt(
        Ord ord) const -> Count {
    const auto cumulativeNumFound = this->_cumulativeFoundWordsCnt.get(ord - 1);
    if (this->_maxOrd == this->_foundWordsCount.size()) {
        return cumulativeNumFound;
    }
    const auto numUniqueWordsTotal = this->_getTotalUniqueWordsCnt();
    const auto cumulativeUniqueWordsNumFound =
            this->_cumulativeFoundUniqueWords.get(ord - 1);
    return (this->_maxOrd - numUniqueWordsTotal) * cumulativeNumFound
            + (ord - cumulativeUniqueWordsNumFound);
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveADictionary<OrdT>::_getWordCnt(Ord ord) const -> Count {
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    if (this->_maxOrd == totalUniqueWordsCnt) {
        return this->_foundWordsCount.at(ord);
    }
    return this->_foundWordsCount.contains(ord)
            ? this->_foundWordsCount.at(ord)
              * (this->_maxOrd - totalUniqueWordsCnt)
            : 1;
}

}

#endif // ADAPTIVE_A_DICTIONARY_HPP
