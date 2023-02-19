#include <ael/dictionary/ppmd_plus_dictionary.hpp>

#include <boost/multiprecision/cpp_int.hpp>

namespace ael::dict {

namespace bmp = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
PPMDPlusDictionary::PPMDPlusDictionary(std::uint16_t wordNumBits,
                               std::uint16_t contextLength,
                               std::uint16_t contextCellBitsLength)
    : AdaptiveDDictionary(1ull << wordNumBits),
      _ctx(0),
      _currCtxLength(0),
      _numBits(wordNumBits),
      _ctxCellBitsLength(contextCellBitsLength),
      _ctxLength(contextLength) {
    if (contextCellBitsLength * contextLength > 56) {
        throw std::invalid_argument("Too big context length.");
    }
}

//----------------------------------------------------------------------------//
auto PPMDPlusDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    for (std::uint16_t ctxLength = _currCtxLength; ctxLength != 0; --ctxLength) {
        const auto ctx = _ctx % (1ull << (_ctxCellBitsLength * ctxLength));
        if (_contextProbs.contains({ctxLength, ctx})
                && _contextProbs.at({ctxLength, ctx})._totalFoundWordsCnt >= ctxLength) {
            return _contextProbs.at({ctxLength, ctx}).getWordOrd(cumulativeNumFound);
        }
    }
    return AdaptiveDDictionary::getWordOrd(cumulativeNumFound);
}

//----------------------------------------------------------------------------//
auto PPMDPlusDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    std::optional<ProbabilityStats> ret;

    for (std::uint16_t ctxLength = _currCtxLength; ctxLength != 0; --ctxLength) {
        const auto ctx = _ctx % (1ull << (_ctxCellBitsLength * ctxLength));
        if (_contextProbs.contains({ctxLength, ctx})) {
            if (!ret.has_value()
                    && _contextProbs.at({ctxLength, ctx})._totalFoundWordsCnt >= ctxLength) {
                ret = _contextProbs.at({ctxLength, ctx})._getProbabilityStats(ord);
            }
        } else {
            _contextProbs.emplace(_SearchCtx{ctxLength, ctx}, this->_maxOrd);
        }
        _contextProbs.at({ctxLength, ctx})._updateWordCnt(ord, 1);
    }
    ret = ret.value_or(this->_getProbabilityStats(ord));
    this->_updateWordCnt(ord, 1);
    _updateCtx(ord);
    return ret.value();
}

//----------------------------------------------------------------------------//
auto PPMDPlusDictionary::getTotalWordsCnt() const -> Count {
    for (std::uint16_t ctxLength = _currCtxLength; ctxLength != 0; --ctxLength) {
        const auto ctx = _ctx % (1ull << (_ctxCellBitsLength * ctxLength));
        if (_contextProbs.contains({ctxLength, ctx})
                && _contextProbs.at({ctxLength, ctx})._totalFoundWordsCnt >= ctxLength) {
            return _contextProbs.at({ctxLength, ctx}).getTotalWordsCnt();
        }
    }
    return AdaptiveDDictionary::getTotalWordsCnt();
}

//----------------------------------------------------------------------------//
void PPMDPlusDictionary::_updateCtx(Ord ord) {
    if (_currCtxLength < _ctxLength) {
        ++_currCtxLength;
    }
    bmp::uint128_t newCtx128 = (_ctx != 0) ? _ctx - 1 : 0;
    newCtx128 *= (1ull << _numBits);
    newCtx128 += ord;
    newCtx128 %= (1ull << (_ctxCellBitsLength * _ctxLength));
    _ctx = newCtx128.convert_to<Ord>();
}

}
