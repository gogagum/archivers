#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"

#include <cassert>
#include <cstdint>
#include <vector>
#include <ranges>

#include <boost/icl/interval_map.hpp>
#include <boost/concept_check.hpp>
#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/upper_bound.hpp>
#include <boost/fusion/view/transform_view.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
template <class WordT>
class AdaptiveDictionary {
public:
    static constexpr bool supportsIncrease = true;
    static constexpr bool requireSymsCounts = false;
    static constexpr bool constructsFromNoArgs = true;
public:
    AdaptiveDictionary();

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    WordT getWord(std::uint64_t cumulativeNumFound) const;

    /**
     * @brief getLowerCumulativeNumFound - lower letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    std::uint64_t getLowerCumulativeNumFound(const WordT& word) const;

    /**
     * @brief getHigherCumulativeNumFound - lower or equal letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    std::uint64_t getHigherCumulativeNumFound(const WordT& word) const;

    /**
     * @brief totalWordsCount
     * @return
     */
    std::uint64_t totalWordsCount() const;

    /**
     * @brief increaseWordCount
     * @param word - word which count we increase.
     */
    void increaseWordCount(const WordT& word);

public:
    boost::icl::interval_map<std::uint64_t, std::int64_t> _additionalCounts;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
AdaptiveDictionary<WordT>::AdaptiveDictionary() {}

//----------------------------------------------------------------------------//
template <class WordT, std::unt8_t baseRatio = 1>
WordT
AdaptiveDictionary<WordT>::getWord(std::uint64_t cumulativeNumFound) const {
    auto beg = misc::IntegerRandomAccessIterator<std::uint64_t>(0);
    auto end = misc::IntegerRandomAccessIterator<std::uint64_t>(WordT::wordsCount);

    auto indexes = boost::make_iterator_range(beg, end);

    static_assert(std::ranges::random_access_range<decltype(indexes)>);

    auto cumulativeNumFoundRng =
            std::ranges::views::transform(
                indexes,
                [this](std::uint64_t index) -> std::uint64_t {
                    return this->_additionalCounts(index) + index + 1;
                });

    auto it = std::ranges::upper_bound(cumulativeNumFoundRng, cumulativeNumFound);
    static_assert(sizeof(decltype(it - cumulativeNumFoundRng.begin())) == 8);

    return WordT::byOrd(it - cumulativeNumFoundRng.begin());
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
AdaptiveDictionary<WordT>::getLowerCumulativeNumFound(const WordT& word) const {
    if (std::uint64_t ord = WordT::ord(word); ord == 0) {
        return 0;
    } else {
        return ord + _additionalCounts(ord - 1);
    }
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
AdaptiveDictionary<WordT>::getHigherCumulativeNumFound(const WordT& word) const {
    std::uint64_t ord = WordT::ord(word);
    return ord + 1 + _additionalCounts(ord);
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t AdaptiveDictionary<WordT>::totalWordsCount() const {
    return WordT::wordsCount + _additionalCounts(WordT::wordsCount - 1);
}

//----------------------------------------------------------------------------//
template <class WordT>
void
AdaptiveDictionary<WordT>::increaseWordCount(const WordT& word) {
    _additionalCounts +=
            std::make_pair(
                boost::icl::interval<std::uint64_t>::right_open(
                    WordT::ord(word),
                    WordT::wordsCount
                ),
                std::int64_t(1)
            );
}

}  // namecpace ga::dict

#endif // ADAPTIVE_DICTIONARY_HPP
