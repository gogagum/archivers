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
template <std::integral OrdT = std::uint64_t>
class AdaptiveDDictionary : protected impl::ADDictionaryBase<OrdT, std::uint64_t> {
public:
    using Ord = OrdT;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
public:
    AdaptiveDDictionary(OrdT maxOrd) : impl::ADDictionaryBase<OrdT, std::uint64_t>(maxOrd) {}
    AdaptiveDDictionary(AdaptiveDDictionary<OrdT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

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

    ProbabilityStats _getProbabilityStats(Ord ord) const;

private:
    template <std::integral _Ord>
    friend class PPMDDictionary;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::integral OrdT>
OrdT AdaptiveDDictionary<OrdT>::getWordOrd(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDDictionary<OrdT>::getProbabilityStats(OrdT ord) -> ProbabilityStats {
    auto ret = _getProbabilityStats(ord);
    this->_updateWordCnt(ord, 1);
    return ret;
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDDictionary<OrdT>::getTotalWordsCnt() const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return this->_maxOrd;
    }
    if (this->_getTotalUniqueWordsCnt() == this->_maxOrd) {
        return this->_totalFoundWordsCnt;
    }
    return 2 * (this->_maxOrd - this->_getTotalUniqueWordsCnt())
            * this->_totalFoundWordsCnt;
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDDictionary<OrdT>::_getLowerCumulativeCnt(Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return ord;
    }
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    const auto cumulativeWordsCnt = this->_cumulativeFoundWordsCnt.get(ord - 1);
    if (totalUniqueWordsCnt == this->_maxOrd) {
        return cumulativeWordsCnt;
    }
    const auto cumulativeUniqueWordsCnt = this->_cumulativeFoundUniqueWords.get(ord - 1);
    return (this->_maxOrd - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt
            + ord * totalUniqueWordsCnt
            - this->_maxOrd * cumulativeUniqueWordsCnt;
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDDictionary<OrdT>::_getWordCnt(Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return 1;
    }
    const auto totalUniqueWordsCount = this->_getTotalUniqueWordsCnt();
    if (totalUniqueWordsCount == this->_maxOrd) {
        return this->_foundWordsCount.at(ord);
    }
    const auto realWordCount = this->_getRealWordCnt(ord);
    return (this->_maxOrd - totalUniqueWordsCount) * 2 * realWordCount
        + totalUniqueWordsCount
        - this->_maxOrd * ((realWordCount > 0) ? 1 : 0);
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDDictionary<OrdT>::_getProbabilityStats(
        Ord ord) const -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    return { low, high, total };
}

}

#endif // ADAPTIVE_D_DICTIONARY_HPP
