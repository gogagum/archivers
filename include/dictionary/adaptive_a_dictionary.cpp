#include "adaptive_a_dictionary.hpp"

#include "integer_random_access_iterator.hpp"
#include <boost/range/iterator_range.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
AdaptiveADictionary::AdaptiveADictionary(Ord maxOrd)
    : impl::ADDictionaryBase<std::uint64_t>(maxOrd) {}

//----------------------------------------------------------------------------//
auto AdaptiveADictionary::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
auto AdaptiveADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    this->_updateWordCnt(ord, 1);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
auto AdaptiveADictionary::getTotalWordsCnt() const -> Count {
    const auto uniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    if (this->_maxOrd == uniqueWordsCnt) {
        return this->_totalFoundWordsCnt;
    }
    return (this->_maxOrd - uniqueWordsCnt)
            * (this->_totalFoundWordsCnt + 1);
}

//----------------------------------------------------------------------------//
auto AdaptiveADictionary::_getLowerCumulativeCnt(
        Ord ord) const -> Count {
    const auto cumulativeNumFound = this->_cumulativeFoundWordsCnt.get(ord - 1);
    if (this->_maxOrd == this->_foundWordsCount.size()) {
        return cumulativeNumFound;
    }
    const auto numUniqueWordsTotal = this->_getTotalUniqueWordsCnt();
    const auto cumulativeUniqueWordsNumFound =
        this->_getLowerCumulativeUniqueNumFound(ord);
    return (this->_maxOrd - numUniqueWordsTotal) * cumulativeNumFound
            + (ord - cumulativeUniqueWordsNumFound);
}

//----------------------------------------------------------------------------//
auto AdaptiveADictionary::_getWordCnt(Ord ord) const -> Count {
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    if (this->_maxOrd == totalUniqueWordsCnt) {
        return this->_foundWordsCount.at(ord);
    }
    return this->_foundWordsCount.contains(ord)
            ? this->_foundWordsCount.at(ord)
              * (this->_maxOrd - totalUniqueWordsCnt)
            : 1;
}

}  // namespace ga::dict
