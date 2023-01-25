#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "word_probability_stats.hpp"

#include <cassert>
#include <cstdint>
#include <vector>
#include <ranges>
#include <iterator>

#include <boost/icl/interval_map.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/irange.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace ga::dict {

namespace bicl = boost::icl;

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class AdaptiveDictionary {
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
    WordT getWord(Count cumulativeNumFound) const;

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
    Count getTotalWordsCnt() const;

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    void _updateCumulativeNumWords(Ord ord);

private:

    using OrdInterval = typename bicl::interval_map<Ord, Count>::interval_type;

private:
    bicl::interval_map<Ord, Count> _cumulativeWordCounts;
    std::unordered_map<Ord, Count> _wordCounts;
    Count _totalWordsCount;
    Count _ratio;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveDictionary<WordT, CountT>::AdaptiveDictionary(std::uint64_t ratio)
    : _totalWordsCount(WordT::wordsCount), _ratio(ratio) {}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
AdaptiveDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return _getLowerCumulativeNumFound(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return WordT::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDictionary<WordT, CountT>::getTotalWordsCnt() const -> Count {
    return _totalWordsCount;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDictionary<WordT, CountT>::getProbabilityStats(
        const WordT& word) -> ProbabilityStats {
    auto ord = WordT::ord(word);
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = low + _wordCounts[ord] * _ratio + 1;
    auto total = getTotalWordsCnt();
    _updateCumulativeNumWords(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void
AdaptiveDictionary<WordT, CountT>::_updateCumulativeNumWords(Ord ord) {
    auto interval = OrdInterval(ord, WordT::wordsCount);
    _cumulativeWordCounts += std::make_pair(interval, Count{1});
    ++_wordCounts[ord];
    _totalWordsCount += _ratio;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    return ord + _cumulativeWordCounts(ord - 1) * _ratio;
}


}  // namecpace ga::dict

#endif // ADAPTIVE_DICTIONARY_HPP
