#ifndef BASE_DICTIONARY_HPP
#define BASE_DICTIONARY_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseDictionary class
///
template <class WordT>
class BaseDictionary {
protected:
    BaseDictionary();

    BaseDictionary(std::vector<std::uint64_t>&& cumulativeNumFound);

public:
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

protected:
    std::vector<std::uint64_t> _cumulativeNumFound;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
BaseDictionary<WordT>::BaseDictionary() {}

//----------------------------------------------------------------------------//
template <class WordT>
BaseDictionary<WordT>::BaseDictionary(
        std::vector<std::uint64_t>&& cumulativeNumFound)
    : _cumulativeNumFound(std::move(cumulativeNumFound)){}

//----------------------------------------------------------------------------//
template <class WordT>
WordT
BaseDictionary<WordT>::getWord(std::uint64_t cumulativeNumFound) const {
    auto it = _cumulativeNumFound.begin();
    bool found = false;

    while (!found) {
        std::ptrdiff_t offset = 1;
        while ((it + offset * 2 - 1) < _cumulativeNumFound.end()
               && *(it + offset * 2 - 1) <= cumulativeNumFound) {
            offset *= 2;
        }
        it += (offset - 1);
        found = (offset == 1);
    }
    assert(found && "Could not find symbol.");

    std::uint64_t ord = it - _cumulativeNumFound.begin();
    return WordT::byOrd(ord);
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
BaseDictionary<WordT>::getLowerCumulativeNumFound(const WordT& word) const {
    const std::uint64_t ord = WordT::ord(word);
    if (ord == 0) {
        return 0;
    }

    return _cumulativeNumFound[ord - 1];
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
BaseDictionary<WordT>::getHigherCumulativeNumFound(const WordT& word) const {
    return _cumulativeNumFound[WordT::ord(word)];
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t BaseDictionary<WordT>::totalWordsCount() const {
    return *_cumulativeNumFound.rbegin();
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t BaseDictionary<WordT>::numUniqueWords() const {
    std::uint64_t prevCumulativeNumFound = 0;
    return std::ranges::count_if(_cumulativeNumFound,
                                 [&prevCumulativeNumFound](auto count) {
                                     auto ret = count != prevCumulativeNumFound;
                                     prevCumulativeNumFound = count;
                                     return ret;
                                 });
}

}  // namespace ga::dict

#endif // BASE_DICTIONARY_HPP
