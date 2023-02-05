#ifndef BASE_A_D_DICTIONARY_HPP
#define BASE_A_D_DICTIONARY_HPP

#include <dst/dynamic_segment_tree.hpp>

namespace ga::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseADDictionary class
///
template <typename OrdT, typename CountT, OrdT ordEnd>
class ADDictionaryBase {
protected:
    using Ord = OrdT;
    using Count = CountT;
protected:
    ADDictionaryBase()
        : _cumulativeFoundUniqueWords(0, ordEnd, 0),
          _totalFoundWordsCnt(0),
          _cumulativeFoundWordsCnt(0, ordEnd, 0) {}

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
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <typename OrdT, typename CountT, OrdT ordEnd>
auto
ADDictionaryBase<OrdT, CountT, ordEnd>::_getTotalUniqueWordsCnt(
        ) const -> Count {
    return _foundWordsCount.size();
}

//----------------------------------------------------------------------------//
template <typename OrdT, typename CountT, OrdT ordEnd>
auto
ADDictionaryBase<OrdT, CountT, ordEnd>::_getLowerCumulativeUniqueNumFound(
        Ord ord) const -> Count {
    return _cumulativeFoundUniueWords(ord - 1);
}

//----------------------------------------------------------------------------//
template <typename OrdT, typename CountT, OrdT ordEnd>
auto ADDictionaryBase<OrdT, CountT, ordEnd>::_getRealWordCnt(
        Ord ord) const -> Count {
    return _foundWordsCount.contains(ord) ? _foundWordsCount.at(ord) : 0;
}

//----------------------------------------------------------------------------//
template <typename OrdT, typename CountT, OrdT ordEnd>
void
ADDictionaryBase<OrdT, CountT, ordEnd>::_updateWordCnt(Ord ord, Count cnt) {
    _cumulativeFoundWordsCnt.update(ord, ordEnd, 1);
    if (!_foundWordsCount.contains(ord)) {
        _cumulativeFoundUniqueWords.update(ord, ordEnd, 1);
    }
    ++_totalFoundWordsCnt;
    ++_foundWordsCount[ord];
}

}

#endif // BASE_A_D_DICTIONARY_HPP
