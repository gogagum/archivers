#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <ranges>

#include <ael/dictionary/integer_random_access_iterator.hpp>
#include <boost/range/iterator_range.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(
        Ord maxOrd,
        Count count
        ) : impl::AdaptiveDictionaryBase<Count>(maxOrd, maxOrd * count),
            _maxOrd(maxOrd) {
    for (auto ord : boost::irange<Ord>(0, _maxOrd)) {
        this->_wordCnts[ord] = count;
        this->_cumulativeWordCounts.update(ord, _maxOrd, count);
    }
}

//----------------------------------------------------------------------------//
auto DecreasingOnUpdateDictionary::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, _maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeNumFound(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
auto DecreasingOnUpdateDictionary::getProbabilityStats(
        Ord ord) -> ProbabilityStats {
    if (!this->_wordCnts.contains(ord)
            || this->_wordCnts.at(ord) == Count(0)) {
        throw NoSuchWord(ord);
    }
    const auto low = _getLowerCumulativeNumFound(ord);
    const auto high = low + this->_wordCnts[ord];
    const auto total = getTotalWordsCnt();
    _updateWordCnt(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
auto DecreasingOnUpdateDictionary::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (ord == 0) {
        return Count{0};
    }
    return this->_cumulativeWordCounts.get(ord - 1);
}

//----------------------------------------------------------------------------//
void DecreasingOnUpdateDictionary::_updateWordCnt(Ord ord) {
    this->_totalWordsCnt -= 1;
    this->_cumulativeWordCounts.update(ord, _maxOrd, - 1);;
    --this->_wordCnts[ord];
}

}  // namespace ga::dict
