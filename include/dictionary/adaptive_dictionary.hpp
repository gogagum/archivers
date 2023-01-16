#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "integer_random_access_iterator.hpp"
#include "../byte_data_constructor.hpp"

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

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class AdaptiveDictionary {
public:

    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;

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
     * @brief getLowerCumulativeNumFound - lower letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    Count getLowerCumulativeNumFound(const WordT& word) const;

    /**
     * @brief getHigherCumulativeNumFound - lower or equal letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    Count getHigherCumulativeNumFound(const WordT& word) const;

    /**
     * @brief totalWordsCount
     * @return
     */
    Count totalWordsCount() const;

    /**
     * @brief increaseWordCount
     * @param word - word which count we increase.
     */
    void increaseWordCount(const WordT& word);

    /**
     * @brief serialize
     */
    void serialize(ByteDataConstructor& dataConstructor) const;

public:
    boost::icl::interval_map<Ord, Count> _additionalCounts;
    Count _ratio;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
AdaptiveDictionary<WordT, CountT>::AdaptiveDictionary(std::uint64_t ratio) : _ratio(ratio) {}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT
AdaptiveDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) const {
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
template <class WordT, typename CountT>
auto
AdaptiveDictionary<WordT, CountT>::getLowerCumulativeNumFound(
        const WordT& word) const -> Count {
    if (std::uint64_t ord = WordT::ord(word); ord == 0) {
        return 0;
    } else {
        return ord + _additionalCounts(ord - 1);
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
AdaptiveDictionary<WordT, CountT>::getHigherCumulativeNumFound(
        const WordT& word) const -> Count {
    std::uint64_t ord = WordT::ord(word);
    return ord + 1 + _additionalCounts(ord);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto AdaptiveDictionary<WordT, CountT>::totalWordsCount() const -> Count {
    return WordT::wordsCount + _additionalCounts(WordT::wordsCount - 1);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void
AdaptiveDictionary<WordT, CountT>::increaseWordCount(const WordT& word) {
    _additionalCounts +=
            std::make_pair(
                boost::icl::interval<Ord>::right_open(
                    WordT::ord(word),
                    WordT::wordsCount
                ),
                _ratio
            );
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void
AdaptiveDictionary<WordT, CountT>::serialize(
        ByteDataConstructor& dataConstructor) const {
    dataConstructor.putT(CountT(_ratio));
}

}  // namecpace ga::dict

#endif // ADAPTIVE_DICTIONARY_HPP
