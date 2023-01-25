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
template <class WordT, typename CountT = std::uint64_t>
class AdaptiveDictionary
        : public impl::AdaptiveDictionaryBase<typename WordT::Ord, CountT> {
public:

    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;

public:
    AdaptiveDictionary(std::uint64_t ratio);

    AdaptiveDictionary(AdaptiveDictionary<WordT, CountT>&& other) = default;

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] WordT getWord(Count cumulativeNumFound) const;

    /**
     * @brief getProbabilityStats reads probability statistics, updates them,
     * and returns read.
     * @param word
     * @return probability estimation for a word.
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(const WordT& word);

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return
     */
    [[nodiscard]] Count getTotalWordsCnt() const { return this->_totalWordsCnt; }

private:

    using OrdInterval =
        typename impl::AdaptiveDictionaryBase<Ord, CountT>::OrdInterval;

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    void _updateWordCnt(Ord ord);

private:
    Count _ratio;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveDictionary<WordT, CountT>::AdaptiveDictionary(std::uint64_t ratio)
    : impl::AdaptiveDictionaryBase<Ord, Count>(WordT::wordsCount)
    , _ratio(ratio) {}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
AdaptiveDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);

    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return _getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return WordT::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDictionary<WordT, CountT>::getProbabilityStats(
        const WordT& word) -> ProbabilityStats {
    const auto ord = WordT::ord(word);
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + this->_wordCnts[ord] * _ratio + 1;
    const auto total = getTotalWordsCnt();
    _updateWordCnt(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void AdaptiveDictionary<WordT, CountT>::_updateWordCnt(Ord ord) {
    const auto interval = OrdInterval(ord, WordT::wordsCount);
    this->_cumulativeWordCounts += std::make_pair(interval, Count{1});
    ++this->_wordCnts[ord];
    this->_totalWordsCnt += _ratio;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDictionary<WordT, CountT>::_getLowerCumulativeCnt(
        Ord ord) const -> Count {
    return ord + this->_cumulativeWordCounts(ord - 1) * _ratio;
}


}  // namecpace ga::dict

#endif // ADAPTIVE_DICTIONARY_HPP
