#ifndef STATIC_DICTIONARY_HPP
#define STATIC_DICTIONARY_HPP

#include "word_probability_stats.hpp"

#include <vector>
#include <boost/range/irange.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
template <class WordT, class CountT = std::uint64_t>
class StaticDictionary {
public:

    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<CountT>;

private:

    StaticDictionary() = default;

public:

    StaticDictionary(const StaticDictionary& other) = default;
    StaticDictionary(StaticDictionary&& other) = default;

    /**
     * @brief fromCumulativeCounts construct from cumulative counts.
     * @param cumulCountsRng - cumulative count of each symbol ordered.
     * @return
     */
    template <class RangeT>
    static StaticDictionary<WordT, CountT> fromCumulativeCounts(const RangeT& cumulCountsRng);

    /**
     * @brief fromCounts construct from simple wordsCounts
     * @param countsRng - count of each symbol ordered.
     * @return
     */
    template <class RangeT>
    static StaticDictionary<WordT, CountT> fromCounts(const RangeT& countsRng);

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    WordT getWord(Count cumulativeNumFound) const;

    /**
     * @brief getProbabilityStats - get lower cumulative number of words,
     * higher cumulative number of words and total count.
     * @param word - word to get info for.
     * @return statistics.
     */
    ProbabilityStats getProbabilityStats(const Word& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    Ord totalWordsCount() const;

    /**
     * @brief serialize
     * @param res
     */
    template <class DestT>
    void serialize(DestT& res) const;

private:

    std::uint64_t _getLowerCumulativeNumFound(Ord ord) const;
    std::uint64_t _getHigherCumulativeNumFound(Ord ord) const;

protected:
    std::vector<Count> _cumulativeNumFound;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class RangeT>
StaticDictionary<WordT, CountT>
StaticDictionary<WordT, CountT>::fromCumulativeCounts(
        const RangeT& cumulativeCountsRange) {
    auto ret = StaticDictionary<WordT, CountT>();
    std::copy(cumulativeCountsRange.begin(),
              cumulativeCountsRange.end(),
              std::back_inserter(ret._cumulativeNumFound));
    return ret;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class RangeT>
StaticDictionary<WordT, CountT>
StaticDictionary<WordT, CountT>::fromCounts(
        const RangeT& numFoundRange) {
    auto ret = StaticDictionary<WordT, CountT>();
    auto currentSum = Count{0};
    for (auto numFound: numFoundRange) {
        currentSum += numFound;
        ret._cumulativeNumFound.emplace_back(currentSum);
    }
    return ret;
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
StaticDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
    auto it = std::ranges::upper_bound(_cumulativeNumFound, cumulativeNumFound);
    return WordT::byOrd(it - _cumulativeNumFound.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
StaticDictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = _getHigherCumulativeNumFound(ord);
    return { low, high, *_cumulativeNumFound.rbegin() };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
std::uint64_t
StaticDictionary<WordT, CountT>::_getLowerCumulativeNumFound(Ord ord) const {
    if (ord == 0) {
        return 0;
    } else {
        return _cumulativeNumFound[ord - 1];
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
std::uint64_t
StaticDictionary<WordT, CountT>::_getHigherCumulativeNumFound(Ord ord) const {
    return _cumulativeNumFound[ord];
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto StaticDictionary<WordT, CountT>::totalWordsCount() const -> typename Word::Ord {
    return *_cumulativeNumFound.rbegin();
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class DestT>
void StaticDictionary<WordT, CountT>::serialize(DestT& res) const {
    res.template putT<std::uint32_t>(this->numUniqueWords());

    // Unique words and their counts
    for (auto i : boost::irange<std::size_t>(0, this->_cumulativeNumFound.size())) {
        auto w = WordT::byOrd(i);
        res.template putT<WordT>(w);
        res.template putT<std::uint64_t>(this->_cumulativeNumFound[i]);
    }
}

}  // namespace ga::dict

#endif // STATIC_DICTIONARY_HPP
