#include <ael/dictionary/adaptive_d_dictionary.hpp>

#include <ael/dictionary/integer_random_access_iterator.hpp>
#include <boost/range/iterator_range.hpp>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
AdaptiveDDictionary::AdaptiveDDictionary(Ord maxOrd)
    : impl::ADDictionaryBase<std::uint64_t>(maxOrd) {}

//----------------------------------------------------------------------------//
auto AdaptiveDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
auto AdaptiveDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    auto ret = _getProbabilityStats(ord);
    this->_updateWordCnt(ord, 1);
    return ret;
}

//----------------------------------------------------------------------------//
auto AdaptiveDDictionary::getTotalWordsCnt() const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return this->_maxOrd;
    }
    if (this->_getTotalUniqueWordsCnt() == this->_maxOrd) {
        return this->_totalFoundWordsCnt;
    }
    return 2 * (this->_maxOrd - this->_getTotalUniqueWordsCnt())
            * this->_totalFoundWordsCnt;
}

//----------------------------------------------------------------------------//
auto AdaptiveDDictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return ord;
    }
    const auto totalUniqueWordsCnt = this->_getTotalUniqueWordsCnt();
    const auto cumulativeWordsCnt = this->_cumulativeFoundWordsCnt.get(ord - 1);
    if (totalUniqueWordsCnt == this->_maxOrd) {
        return cumulativeWordsCnt;
    }
    const auto cumulativeUniqueWordsCnt
        = this->_getLowerCumulativeUniqueNumFound(ord); 
    return (this->_maxOrd - totalUniqueWordsCnt) * 2 * cumulativeWordsCnt
            + ord * totalUniqueWordsCnt
            - this->_maxOrd * cumulativeUniqueWordsCnt;
}

//----------------------------------------------------------------------------//
auto AdaptiveDDictionary::_getWordCnt(Ord ord) const -> Count {
    if (this->_totalFoundWordsCnt == 0) {
        return 1;
    }
    const auto totalUniqueWordsCount = this->_getTotalUniqueWordsCnt();
    if (totalUniqueWordsCount == this->_maxOrd) {
        return this->_foundWordsCount.at(ord);
    }
    const auto realWordCount = this->_getRealWordCnt(ord);
    return (this->_maxOrd - totalUniqueWordsCount) * 2 * realWordCount
        + totalUniqueWordsCount
        - this->_maxOrd * ((realWordCount > 0) ? 1 : 0);
}

//----------------------------------------------------------------------------//
auto AdaptiveDDictionary::_getProbabilityStats(
        Ord ord) const -> ProbabilityStats {
    const auto low = _getLowerCumulativeCnt(ord);
    const auto high = low + _getWordCnt(ord);
    const auto total = getTotalWordsCnt();
    return { low, high, total };
}

}  // namespace ga::dict
