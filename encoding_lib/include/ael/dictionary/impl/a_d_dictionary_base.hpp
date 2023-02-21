#ifndef BASE_A_D_DICTIONARY_HPP
#define BASE_A_D_DICTIONARY_HPP

#include <dst/dynamic_segment_tree.hpp>

namespace ael::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseADDictionary class
///
template <typename CountT>
class ADDictionaryBase {
protected:
    using Ord = std::uint64_t;
    using Count = CountT;
protected:
    ADDictionaryBase(Ord maxOrd)
        : _cumulativeFoundUniqueWords(0, maxOrd, 0),
          _totalFoundWordsCnt(0),
          _cumulativeFoundWordsCnt(0, maxOrd, 0),
          _maxOrd(maxOrd) {}

    Count _getTotalUniqueWordsCnt() const;

    Count _getLowerCumulativeUniqueNumFound(Ord ord) const;

    Count _getRealWordCnt(Ord ord) const;

    void _updateWordCnt(Ord ord, Count cnt);

protected:

    using DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;

protected:
    DST _cumulativeFoundWordsCnt;
    Count _totalFoundWordsCnt;
    DST _cumulativeFoundUniqueWords;
    std::unordered_map<Ord, Count> _foundWordsCount;
    const Ord _maxOrd;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <typename CountT>
auto ADDictionaryBase<CountT>::_getTotalUniqueWordsCnt() const -> Count {
    return _foundWordsCount.size();
}

//----------------------------------------------------------------------------//
template <typename CountT>
auto ADDictionaryBase<CountT>::_getLowerCumulativeUniqueNumFound(
        Ord ord) const -> Count {
    return _cumulativeFoundUniqueWords.get(ord - 1);
}

//----------------------------------------------------------------------------//
template <typename CountT>
auto ADDictionaryBase<CountT>::_getRealWordCnt(Ord ord) const -> Count {
    return _foundWordsCount.contains(ord) ? _foundWordsCount.at(ord) : 0;
}

//----------------------------------------------------------------------------//
template <typename CountT>
void ADDictionaryBase<CountT>::_updateWordCnt(Ord ord, Count cnt) {
    _cumulativeFoundWordsCnt.update(ord, _maxOrd, cnt);
    if (!_foundWordsCount.contains(ord)) {
        _cumulativeFoundUniqueWords.update(ord, _maxOrd, 1);
    }
    _totalFoundWordsCnt += cnt;
    _foundWordsCount[ord] += cnt;
}

}

#endif // BASE_A_D_DICTIONARY_HPP
