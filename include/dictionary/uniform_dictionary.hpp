#ifndef UNIFORM_DICTIONARY_HPP
#define UNIFORM_DICTIONARY_HPP

#include "dictionary_tags.hpp"

#include <cstdint>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The UniformDictionary class
///
template <class WordT>
class UniformDictionary {
public:

    using Word = WordT;
    using ConstructionTag = tags::ConstructsFromNoArgs;

public:

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    WordT getWord(std::uint64_t cumulativeNumFound);

    /**
     * @brief getLowerCumulativeNumFound - lower letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    std::uint64_t getLowerCumulativeNumFound(const WordT& word);

    /**
     * @brief getHigherCumulativeNumFound - lower or equal letters count.
     * @param word - key to search.
     * @return cumulative lower words count.
     */
    std::uint64_t getHigherCumulativeNumFound(const WordT& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    std::uint64_t totalWordsCount() const;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
WordT UniformDictionary<WordT>::getWord(std::uint64_t cumulativeNumFound) {
    return WordT::byOrd(cumulativeNumFound);
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
UniformDictionary<WordT>::getLowerCumulativeNumFound(const WordT& word) {
    return WordT::ord(word);
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t
UniformDictionary<WordT>::getHigherCumulativeNumFound(const WordT& word) {
    return WordT::ord(word) + 1;
}

//----------------------------------------------------------------------------//
template <class WordT>
std::uint64_t UniformDictionary<WordT>::totalWordsCount() const {
    return WordT::wordsCount;
}

}  // namespace ga::dict

#endif // UNIFORM_DICTIONARY_HPP
