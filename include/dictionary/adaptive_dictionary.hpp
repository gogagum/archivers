#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"
#include "impl/adaptive_dictionary_base.hpp"

#include <cassert>
#include <cstdint>
#include <vector>
#include <ranges>
#include <iterator>

#include <boost/range/iterator_range.hpp>
#include <boost/range/irange.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
template <std::integral OrdT = std::uint64_t>
class AdaptiveDictionary
        : public impl::AdaptiveDictionaryBase<OrdT, std::uint64_t> {
public:

    using Ord = OrdT;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;

public:

    AdaptiveDictionary(OrdT maxOrd, std::uint64_t ratio);
    AdaptiveDictionary(AdaptiveDictionary<OrdT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getProbabilityStats reads probability statistics, updates them,
     * and returns read.
     * @param word
     * @return probability estimation for a word.
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return
     */
    [[nodiscard]] Count getTotalWordsCnt() const { return this->_totalWordsCnt; }

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    void _updateWordCnt(Ord ord);

private:
    Count _ratio;
    OrdT _maxOrder;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::integral OrdT>
AdaptiveDictionary<OrdT>::AdaptiveDictionary(OrdT maxOrd, std::uint64_t ratio)
    : impl::AdaptiveDictionaryBase<Ord, Count>(maxOrd, maxOrd),
      _ratio(ratio),
      _maxOrder(maxOrd) {}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
OrdT AdaptiveDictionary<OrdT>::getWordOrd(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, _maxOrder);

    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return _getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDictionary<OrdT>::getProbabilityStats(OrdT ord) -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + this->_wordCnts[ord] * _ratio + 1;
    const auto total = getTotalWordsCnt();
    _updateWordCnt(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
void AdaptiveDictionary<OrdT>::_updateWordCnt(Ord ord) {
    this->_cumulativeWordCounts.update(ord, _maxOrder, 1);
    ++this->_wordCnts[ord];
    this->_totalWordsCnt += _ratio;
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto AdaptiveDictionary<OrdT>::_getLowerCumulativeCnt(Ord ord) const -> Count {
    return ord + this->_cumulativeWordCounts.get(ord - 1) * _ratio;
}


}  // namecpace ga::dict

#endif // ADAPTIVE_DICTIONARY_HPP
