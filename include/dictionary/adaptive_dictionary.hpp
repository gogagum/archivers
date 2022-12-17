#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"

#include <cassert>
#include <cstdint>
#include <vector>
#include <ranges>
#include <iterator>

#include <boost/icl/interval_map.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/irange.hpp>

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
template <class WordT>
WordT
AdaptiveDictionary<WordT>::getWord(std::uint64_t cumulativeNumFound) const {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](std::uint64_t index) {
        return this->_additionalCounts(index) + index + 1;
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return WordT::byOrd(it - idxs.begin());
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
