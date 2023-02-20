#ifndef CONTEXTUAL_DICTIONARY_BASE_HPP
#define CONTEXTUAL_DICTIONARY_BASE_HPP

#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>
#include <ael/dictionary/word_probability_stats.hpp>
#include <dst/dynamic_segment_tree.hpp>

namespace ael::dict::impl {

namespace bmp = boost::multiprecision;

template<class InternalDictT>
class ContextualDictionaryBase : protected InternalDictT {

public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
private:

    using _DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

    using _DDict = InternalDictT;

    constexpr static std::uint8_t maxContextLength = 8;

private:

    struct _SearchCtx {
        std::uint16_t length;
        Ord ctx;
        friend bool operator==(_SearchCtx, _SearchCtx) = default;
    };

    struct _SearchCtxHash {
        std::size_t operator()(_SearchCtx searchCtx) const {
            return static_cast<std::size_t>(searchCtx.ctx)
                ^ std::size_t{searchCtx.length};
        }
    };

public:

    /**
     * @brief base contextual dictionary constructor.
     * @param wordNumBits - word bits length.
     * @param contextLength - number of c protected:ontext cells.
     * @param contextCellBitsLength - context cell bits length.
     */
    ContextualDictionaryBase(std::uint16_t wordNumBits,
                             std::uint16_t contextLength,
                             std::uint16_t contextCellBitsLength);

    /**
     * @brief getWord
     * @param cumulativeNumFound
     * @return
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount get total number of words according to model.
     * @return totalWordsCount according to dictionary model.
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

private:

    void _updateCtx(Ord ord);

private:
    std::unordered_map<_SearchCtx, _DDict, _SearchCtxHash> _contextProbs;
    Ord _ctx;
    std::uint16_t _currCtxLength;
    const std::uint16_t _numBits;
    const std::uint16_t _ctxCellBitsLength;
    const std::uint16_t _ctxLength;

};

////////////////////////////////////////////////////////////////////////////////
template<class InternalDictT>
ContextualDictionaryBase<InternalDictT>::ContextualDictionaryBase(
    std::uint16_t wordNumBits,
    std::uint16_t contextLength,
    std::uint16_t contextCellBitsLength
    ) : InternalDictT(1ull << wordNumBits),
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
template<class InternalDictT>
auto ContextualDictionaryBase<InternalDictT>::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    for (std::uint16_t ctxLength = _currCtxLength; ctxLength != 0; --ctxLength) {
        const auto ctx = _ctx % (1ull << (_ctxCellBitsLength * ctxLength));
        if (_contextProbs.contains({ctxLength, ctx})) {
            return _contextProbs.at({ctxLength, ctx}).getWordOrd(cumulativeNumFound);
        }
    }
    return InternalDictT::getWordOrd(cumulativeNumFound);
}

//----------------------------------------------------------------------------//
template<class InternalDictT>
auto ContextualDictionaryBase<InternalDictT>::getProbabilityStats(
        Ord ord) -> ProbabilityStats {
    std::optional<ProbabilityStats> ret;

    for (std::uint16_t ctxLength = _currCtxLength; ctxLength != 0; --ctxLength) {
        const auto ctx = _ctx % (1ull << (_ctxCellBitsLength * ctxLength));
        if (_contextProbs.contains({ctxLength, ctx})) {
            if (!ret.has_value()) {
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
template<class InternalDictT>
auto ContextualDictionaryBase<InternalDictT>::getTotalWordsCnt(
        ) const -> Count {
    for (std::uint16_t ctxLength = _currCtxLength; ctxLength != 0; --ctxLength) {
        const auto ctx = _ctx % (1ull << (_ctxCellBitsLength * ctxLength));
        if (_contextProbs.contains({ctxLength, ctx})) {
            return _contextProbs.at({ctxLength, ctx}).getTotalWordsCnt();
        }
    }
    return InternalDictT::getTotalWordsCnt();
}

//----------------------------------------------------------------------------//
template<class InternalDictT>
void ContextualDictionaryBase<InternalDictT>::_updateCtx(Ord ord) {
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

#endif  // CONTEXTUAL_DICTIONARY_BASE_HPP