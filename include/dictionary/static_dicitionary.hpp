#ifndef STATIC_DICITIONARY_HPP
#define STATIC_DICITIONARY_HPP

#include <boost/range/irange.hpp>

#include "base_dictionary.hpp"
#include "../arithmetic_coder_encoded.hpp"

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
    template <class RangeT>
    StaticDictionary(const RangeT& cumulativeWordsCountsRange);

    /**
     * @brief StaticDictionary
     * @param cumulativeWordsCounts
     */
    StaticDictionary(std::vector<std::uint64_t>&& cumulativeWordsCounts);

    template <class CountT>
    void serialize(ArithmeticCoderEncoded& res) const;

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
    : BaseDictionary<WordT>(std::move(cumulativeNumFound)) {
    assert(this->_cumulativeNumFound.size() == WordT::wordsCount
           && "Vector length should be the same as number of all "
              "possible words.");
}

//----------------------------------------------------------------------------//
template <class WordT>
template <class CountT>
void StaticDictionary<WordT>::serialize(ArithmeticCoderEncoded& res) const {
    res.putT<std::uint32_t>(this->numUniqueWords());

    // Unique words and their counts
    for (auto i : boost::irange<std::size_t>(0, this->_cumulativeNumFound.size())) {
        auto w = WordT::byOrd(i);
        res.putT<WordT>(w);
        res.putT<CountT>(this->_cumulativeNumFound[i]);
    }
}

}  // namespace ga::dict

#endif // STATIC_DICITIONARY_HPP
