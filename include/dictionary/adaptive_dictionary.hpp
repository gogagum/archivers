#ifndef ADAPTIVE_DICTIONARY_HPP
#define ADAPTIVE_DICTIONARY_HPP

#include "base_dictionary.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionary class
///
template <class WordT>
class AdaptiveDictionary : public BaseDictionary<WordT> {
public:
    static constexpr bool supportsIncrease = true;
public:
    AdaptiveDictionary();

    /**
     * @brief increaseWordCount
     * @param word - word which count we increase.
     */
    void increaseWordCount(const WordT& word);
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
AdaptiveDictionary<WordT>::AdaptiveDictionary() {
    for (std::size_t i = 0; i < WordT::wordsCount; ++i) {
        this->_cumulativeNumFound[i] = i + 1;
    }
}

//----------------------------------------------------------------------------//
template <class WordT>
void
AdaptiveDictionary<WordT>::increaseWordCount(const WordT& word) {
    for (std::uint64_t i = WordT::ord(word); i < WordT::wordsCount; ++i) {
        ++this->_cumulativeNumFound[i];
    }
}

}  // namecpace ga::dict

#endif // ADAPTIVE_DICTIONARY_HPP
