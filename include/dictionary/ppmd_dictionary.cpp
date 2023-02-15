#include "ppmd_dictionary.hpp"

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
auto PPMDDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    for (auto ctx = _getInitSearchCtx(); !ctx.empty(); ctx.pop_back()) {
        if (_contextProbs.contains(ctx)) {
            return _contextProbs.at(ctx).getWordOrd(cumulativeNumFound);
        }
    }
    return AdaptiveDDictionary::getWordOrd(cumulativeNumFound);
}

//----------------------------------------------------------------------------//
auto PPMDDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    std::optional<ProbabilityStats> ret;

    for (auto ctx = _getInitSearchCtx(); !ctx.empty(); ctx.pop_back()) {
        if (_contextProbs.contains(ctx)) {
            if (!ret.has_value()) {
                ret = _contextProbs.at(ctx)._getProbabilityStats(ord);
            }
        } else {
            _contextProbs.emplace(ctx, this->_maxOrd);
        }
        _contextProbs.at(ctx)._updateWordCnt(ord, 1);
    }
    ret = ret.value_or(this->_getProbabilityStats(ord));
    this->_updateWordCnt(ord, 1);
    _updateCtx(ord);
    return ret.value();
}

//----------------------------------------------------------------------------//
auto PPMDDictionary::getTotalWordsCnt() const -> Count {
    for (auto ctx = _getInitSearchCtx(); !ctx.empty(); ctx.pop_back()) {
        if (_contextProbs.contains(ctx)) {
            return _contextProbs.at(ctx).getTotalWordsCnt();
        }
    }
    return AdaptiveDDictionary::getTotalWordsCnt();
}

//----------------------------------------------------------------------------//
void PPMDDictionary::_updateCtx(Ord ord) {
    if (_ctx.size() == _contextLen) {
        _ctx.pop_front();
    }
    _ctx.push_back(ord);
}

}
