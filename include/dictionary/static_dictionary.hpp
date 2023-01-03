#ifndef STATIC_DICTIONARY_HPP
#define STATIC_DICTIONARY_HPP

#include <boost/range/irange.hpp>

#include "../byte_data_constructor.hpp"

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
template <class WordT>
class StaticDictionary {
public:
    static constexpr bool supportsIncrease = false;
    static constexpr bool requireSymsCounts = true;
    static constexpr bool constructsFromNoArgs = false;

public:

    /**
     * @brief StaticDictionary
     * @param cumulativeWordsCountsBegin
     * @param cumulativeWordsCountsEnd
     */
    template <class RangeT>
    StaticDictionary(const RangeT& cumulativeWordsCountsRange);

    /**
     * @brief StaticDictionary
     * @param cumulativeWordsCounts
     */
    StaticDictionary(std::vector<std::uint64_t>&& cumulativeWordsCounts);

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
     * @brief numUniqueWords
     * @return
     */
    std::uint64_t numUniqueWords() const;

    template <class CountT>
    void serialize(ByteDataConstructor& res) const;

protected:
    std::vector<std::uint64_t> _cumulativeNumFound;

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
template <class RangeT>
StaticDictionary<WordT>::StaticDictionary(
        const RangeT& cumulativeWordsCountsRange) {
    std::copy(cumulativeWordsCountsRange.begin(),
              cumulativeWordsCountsRange.end(),
              std::back_inserter(this->_cumulativeNumFound));
}

//----------------------------------------------------------------------------//
template <class WordT>
StaticDictionary<WordT>::StaticDictionary(
        std::vector<std::uint64_t>&& cumulativeNumFound)
    : _cumulativeNumFound(std::move(cumulativeNumFound)) {
    assert(this->_cumulativeNumFound.size() == WordT::wordsCount
           && "Vector length should be the same as number of all "
              "possible words.");
}

//----------------------------------------------------------------------------//
template <class WordT>
WordT
StaticDictionary<WordT>::getWord(std::uint64_t cumulativeNumFound) const {
    auto it = std::ranges::upper_bound(_cumulativeNumFound, cumulativeNumFound);
    return WordT::byOrd(it - _cumulativeNumFound.begin());
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
StaticDictionary<WordT>::getLowerCumulativeNumFound(const WordT& word) const {
    if (std::uint64_t ord = WordT::ord(word); ord == 0) {
        return 0;
    } else {
        return _cumulativeNumFound[ord - 1];
    }
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
StaticDictionary<WordT>::getHigherCumulativeNumFound(const WordT& word) const {
    return _cumulativeNumFound[WordT::ord(word)];
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t StaticDictionary<WordT>::totalWordsCount() const {
    return *_cumulativeNumFound.rbegin();
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t StaticDictionary<WordT>::numUniqueWords() const {
    std::uint64_t prevCumulativeNumFound = 0;
    return std::ranges::count_if(_cumulativeNumFound,
                                 [&prevCumulativeNumFound](auto count) {
                                     auto ret = count != prevCumulativeNumFound;
                                     prevCumulativeNumFound = count;
                                     return ret;
                                 });
}

//----------------------------------------------------------------------------//
template <class WordT>
template <class CountT>
void StaticDictionary<WordT>::serialize(ByteDataConstructor& res) const {
    res.putT<std::uint32_t>(this->numUniqueWords());

    // Unique words and their counts
    for (auto i : boost::irange<std::size_t>(0, this->_cumulativeNumFound.size())) {
        auto w = WordT::byOrd(i);
        res.putT<WordT>(w);
        res.putT<CountT>(this->_cumulativeNumFound[i]);
    }
}

}  // namespace ga::dict

#endif // STATIC_DICTIONARY_HPP
