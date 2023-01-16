#ifndef STATIC_DICTIONARY_HPP
#define STATIC_DICTIONARY_HPP

#include <boost/range/irange.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
template <class WordT, class CountT = std::uint64_t>
class StaticDictionary {
public:

    using Word = WordT;
    using Count = CountT;

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
    WordT getWord(Count cumulativeNumFound) const;

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
    typename Word::Ord totalWordsCount() const;

    /**
     * @brief numUniqueWords
     * @return
     */
    std::uint64_t numUniqueWords() const;

    /**
     * @brief serialize
     * @param res
     */
    template <class DestT>
    void serialize(DestT& res) const;

protected:
    std::vector<std::uint64_t> _cumulativeNumFound;

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class RangeT>
StaticDictionary<WordT, CountT>::StaticDictionary(
        const RangeT& cumulativeWordsCountsRange) {
    std::copy(cumulativeWordsCountsRange.begin(),
              cumulativeWordsCountsRange.end(),
              std::back_inserter(this->_cumulativeNumFound));
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
StaticDictionary<WordT, CountT>::StaticDictionary(
        std::vector<std::uint64_t>&& cumulativeNumFound)
    : _cumulativeNumFound(std::move(cumulativeNumFound)) {
    assert(this->_cumulativeNumFound.size() == WordT::wordsCount
           && "Vector length should be the same as number of all "
              "possible words.");
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
std::uint64_t
StaticDictionary<WordT, CountT>::getLowerCumulativeNumFound(const WordT& word) const {
    if (std::uint64_t ord = WordT::ord(word); ord == 0) {
        return 0;
    } else {
        return _cumulativeNumFound[ord - 1];
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
std::uint64_t
StaticDictionary<WordT, CountT>::getHigherCumulativeNumFound(const WordT& word) const {
    return _cumulativeNumFound[WordT::ord(word)];
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto StaticDictionary<WordT, CountT>::totalWordsCount() const -> typename Word::Ord {
    return *_cumulativeNumFound.rbegin();
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
std::uint64_t StaticDictionary<WordT, CountT>::numUniqueWords() const {
    std::uint64_t prevCumulativeNumFound = 0;
    return std::ranges::count_if(_cumulativeNumFound,
                                 [&prevCumulativeNumFound](auto count) {
                                     auto ret = count != prevCumulativeNumFound;
                                     prevCumulativeNumFound = count;
                                     return ret;
                                 });
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
