#ifndef STATIC_DICITIONARY_HPP
#define STATIC_DICITIONARY_HPP

#include "base_dictionary.hpp"

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The StaticDictionary class
///
template <class WordT>
class StaticDictionary : public BaseDictionary<WordT> {
public:
    static constexpr bool supportsIncrease = false;

public:

    /**
     * @brief StaticDictionary
     * @param cumulativeWordsCountsBegin
     * @param cumulativeWordsCountsEnd
     */
    template <class IteratorT>
    StaticDictionary(IteratorT cumulativeWordsCountsBegin,
                     IteratorT cumulativeWordsCountsEnd);
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
template <class IteratorT>
StaticDictionary<WordT>::StaticDictionary(
        IteratorT cumulativeWordsCountsBegin,
        IteratorT cumulativeWordsCountsEnd) {
    std::copy(cumulativeWordsCountsBegin,
              cumulativeWordsCountsEnd,
              std::back_inserter(this->_cumulativeNumFound));
}

}  // namespace ga::dict

#endif // STATIC_DICITIONARY_HPP
