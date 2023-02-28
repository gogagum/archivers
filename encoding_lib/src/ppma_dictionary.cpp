#include <ael/dictionary/ppma_dictionary.hpp>
#include <stdexcept>
#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
PPMADictionary::PPMADictionary(Ord maxOrd, std::size_t ctxLength)
    : _ctxLength(ctxLength),
      AdaptiveADictionary(maxOrd) {
    /**
     * \tau_{ctx}_{i} < sequenceLength
     * Product of tau-s must be less than sequenceLength ^ "tau-s count"
     * Estimation: sequenceLength * l_{ctx} < maxCntBits.
     */
    if (_maxSeqLenLog2 * ctxLength > countNumBits) {
        throw std::logic_error("Too big context.");
    }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    assert(cumulativeNumFound <= this->getTotalWordsCnt());
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        assert(ord < this->_maxOrd);
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    auto t = (it - idxs.begin());
    assert(t < this->_maxOrd);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    assert(ord < this->_maxOrd);
    const auto ret = _getProbabilityStats(ord);
    _updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::getTotalWordsCnt() const -> Count {
    Count total = 0;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto totalCnt = _ctxInfo.at(ctx).getTotalWordsCnt();  
        total = (total + 1) * totalCnt;
    }
    const auto totalCnt = AdaptiveADictionary::getTotalWordsCnt(); 
    total = (total + 1) * totalCnt;
    return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    assert(ord <= this->_maxOrd);
    Count l = 0;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        l *= _ctxInfo.at(ctx).getTotalWordsCnt();
        l += _ctxInfo.at(ctx).getLowerCumulativeCount(ord);
    }
    l *= AdaptiveADictionary::getTotalWordsCnt();
    l += AdaptiveADictionary::_getLowerCumulativeCnt(ord);
    return l;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::_getProbabilityStats(Ord ord) const -> ProbabilityStats {
    Count l = 0;
    Count r = 0;
    Count total = 0;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto totalCnt = _ctxInfo.at(ctx).getTotalWordsCnt();  
        l *= totalCnt;
        r *= totalCnt;
        total = (total + 1) * totalCnt;
        const auto ctxL = _ctxInfo.at(ctx).getLowerCumulativeCount(ord);
        const auto ctxR = ctxL + _ctxInfo.at(ctx).getCount(ord);
        l += ctxL;
        r += ctxR;
    }
    const auto [aDictL, aDictR, aDictTotal] =
        AdaptiveADictionary::_getProbabilityStats(ord);
    l *= aDictTotal;
    r *= aDictTotal;
    total = (total + 1) * aDictTotal;
    l += aDictL;
    r += aDictR;
    assert(r > l);
    assert(total >= r);
    return {l, r, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMADictionary::_updateWordCnt(Ord ord, AdaptiveADictionary::Count cnt) {
    for (auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
         !ctx.empty();
         ctx.pop_back()) {
        if (!_ctxInfo.contains(ctx)) {
            _ctxInfo.emplace(ctx, impl::CumulativeCount(_maxOrd));
        }
        _ctxInfo.at(ctx).increaseOrdCount(ord, cnt);
    }
    AdaptiveADictionary::_updateWordCnt(ord, cnt);
    _ctx.push_back(ord);
    if (_ctx.size() > _ctxLength) {
        _ctx.pop_front();
    }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMADictionary::_getSearchCtxEmptySkipped() const -> _SearchCtx {
    auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
    for (;
         !ctx.empty() && !_ctxInfo.contains(ctx);
         ctx.pop_back()) {
    // Skip contexts which were not found yet.
    }
    return ctx;
}


}