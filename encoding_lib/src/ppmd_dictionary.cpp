#include <ael/dictionary/ppmd_dictionary.hpp>

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <boost/range/iterator_range.hpp>

#include "ael/dictionary/impl/cumulative_count.hpp"
#include "ael/dictionary/impl/cumulative_unique_count.hpp"
#include "integer_random_access_iterator.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
PPMDDictionary::PPMDDictionary(Ord maxOrd, std::size_t ctxLength)
    : _maxOrd(maxOrd),
      _zeroCtxCnt(maxOrd),
      _zeroCtxUniqueCnt(maxOrd),
      _ctxLength(ctxLength) {
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
auto PPMDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    using UintIt = ael::impl::IntegerRandomAccessIterator<std::uint64_t>;
    const auto idxs = boost::make_iterator_range<UintIt>(0, this->_maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    assert(cumulativeNumFound <= this->getTotalWordsCnt());
    const auto getLowerCumulNumFound_ = [this](std::uint64_t ord) {
        assert(ord < _maxOrd);
        return this->_getLowerCumulativeCnt(ord + 1);
    };
    const auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                             getLowerCumulNumFound_);
    auto t = (it - idxs.begin());
    assert(t < _maxOrd);
    return it - idxs.begin();
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    assert(ord < _maxOrd);
    const auto ret = _getProbabilityStats(ord);
    _updateWordCnt(ord, 1);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::getTotalWordsCnt() const -> Count {
    Count total = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto totalCnt = _ctxInfo.at(ctx).getTotalWordsCnt();
        assert(totalCnt != 0);  
        total *= totalCnt * 2;
    }
    if (const auto totalZeroCtxCnt = _zeroCtxCnt.getTotalWordsCnt();
            totalZeroCtxCnt != 0) {
        total *= _zeroCtxCnt.getTotalWordsCnt() * 2;
    }
    if (const auto zeroUniqueCnt = _zeroCtxUniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        total *= this->_maxOrd - zeroUniqueCnt;
    }
    return total;
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::_getLowerCumulativeCnt(Ord ord) const -> Count {
    // TODO
    Count lower = 0;
    Count uniqueCountsProd = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto& ctxInfo = _ctxInfo.at(ctx);
        const auto& ctxUniqueInfo = _ctxUniqueInfo.at(ctx);
        const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
        lower *= ctxTotalCnt * 2;
        lower += (
            ctxInfo.getLowerCumulativeCount(ord)
            - ctxUniqueInfo.getLowerCumulativeCount(ord)
            ) * uniqueCountsProd;
        uniqueCountsProd *= ctxUniqueInfo.getTotalWordsCnt();
    }
    lower *= _zeroCtxCnt.getTotalWordsCnt() * 2;
    lower += (
        _zeroCtxCnt.getLowerCumulativeCount(ord) * 2
        - _zeroCtxUniqueCnt.getLowerCumulativeCount(ord)
    ) * uniqueCountsProd;
    if (const auto zeroUniqueCnt = _zeroCtxUniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        lower *= this->_maxOrd - zeroUniqueCnt;
        lower += ord - _zeroCtxUniqueCnt.getLowerCumulativeCount(ord);
    }
    return {lower};
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::_getProbabilityStats(Ord ord) const -> ProbabilityStats {
    // TODO
    Count lower = 0;
    Count count = 0;
    Count total = 1;
    Count uniqueCountsProd = 1;
    for (auto ctx = _getSearchCtxEmptySkipped();
         !ctx.empty();
         ctx.pop_back()) {
        const auto& ctxInfo = _ctxInfo.at(ctx);
        const auto& ctxUniqueInfo = _ctxInfo.at(ctx);
        const auto ctxTotalCnt = ctxInfo.getTotalWordsCnt();
        lower *= ctxTotalCnt * 2;
        total *= ctxTotalCnt * 2;
        lower += (
                ctxInfo.getLowerCumulativeCount(ord) * 2
                - ctxUniqueInfo.getLowerCumulativeCount(ord)
            ) * uniqueCountsProd;
        count *= ctxTotalCnt * 2;
        count += (
            ctxInfo.getCount(ord) * 2
            - ctxUniqueInfo.getCount(ord)
        ) * uniqueCountsProd;
        uniqueCountsProd *= ctxUniqueInfo.getTotalWordsCnt();
    }
    const auto zeroCtxTotal = _zeroCtxCnt.getTotalWordsCnt();
    if (zeroCtxTotal != 0) {
        lower *= zeroCtxTotal * 2;
        total *= zeroCtxTotal * 2;
        lower += (
            _zeroCtxCnt.getLowerCumulativeCount(ord) * 2
            - _zeroCtxUniqueCnt.getLowerCumulativeCount(ord)
        ) * uniqueCountsProd;
        count *= zeroCtxTotal * 2;
        count += (
            _zeroCtxCnt.getCount(ord) * 2
            - _zeroCtxUniqueCnt.getCount(ord)
        ) * uniqueCountsProd;
        uniqueCountsProd *= _zeroCtxUniqueCnt.getTotalWordsCnt();
    }
    if (const auto zeroUniqueCnt = _zeroCtxUniqueCnt.getTotalWordsCnt();
            zeroUniqueCnt < this->_maxOrd) {
        total *= this->_maxOrd - zeroUniqueCnt;
        lower *= this->_maxOrd - zeroUniqueCnt;
        lower += ord - _zeroCtxUniqueCnt.getLowerCumulativeCount(ord);
        if (count == 0) {
            count = 1;
        } else {
            count *= this->_maxOrd - zeroUniqueCnt;
        }
    }
    assert(count > 0);
    assert(lower + count <= total);
    return {lower, lower + count, total};
}

////////////////////////////////////////////////////////////////////////////////
void PPMDDictionary::_updateWordCnt(Ord ord,
                                    impl::CumulativeCount::Count cnt) {
    for (auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
         !ctx.empty();
         ctx.pop_back()) {
        if (!_ctxInfo.contains(ctx)) {
            _ctxInfo.emplace(ctx, _maxOrd);
        }
        if (!_ctxUniqueInfo.contains(ctx)) {
            _ctxUniqueInfo.emplace(ctx, _maxOrd);
        }
        _ctxInfo.at(ctx).increaseOrdCount(ord, cnt);
        _ctxUniqueInfo.at(ctx).update(ord);
    }
    _zeroCtxCnt.increaseOrdCount(ord, cnt);
    _zeroCtxUniqueCnt.update(ord);
    _ctx.push_back(ord);
    if (_ctx.size() > _ctxLength) {
        _ctx.pop_front();
    }
}

////////////////////////////////////////////////////////////////////////////////
auto PPMDDictionary::_getSearchCtxEmptySkipped() const -> _SearchCtx {
    auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
    for (;
         !ctx.empty() && !_ctxInfo.contains(ctx);
         ctx.pop_back()) {
        // Skip contexts which were not found yet.
    }
    return ctx;
}

}  // namespace ael::dict