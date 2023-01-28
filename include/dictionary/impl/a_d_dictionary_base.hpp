#ifndef BASE_A_D_DICTIONARY_HPP
#define BASE_A_D_DICTIONARY_HPP

#include <boost/icl/interval_map.hpp>

namespace ga::dict::impl {

namespace bicl = boost::icl;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseADDictionary class
///
template <typename OrdT, typename CountT, OrdT ordEnd>
class ADDictionaryBase {
protected:
    using Ord = OrdT;
    using Count = CountT;
protected:
    ADDictionaryBase() : _totalFoundWordsCnt(0) {}

    Count _getTotalUniqueWordsCnt() const;

    Count _getLowerCumulativeUniqueNumFound(Ord ord) const;

    Count _getRealWordCnt(Ord ord) const;

    void _updateWordCnt(Ord ord, Count cnt);

protected:

    using OrdInterval = typename bicl::interval_map<Ord, Count>::interval_type;

protected:
    bicl::interval_map<Ord, Count> _cumulativeFoundWordsCnt;
    Count _totalFoundWordsCnt;
    bicl::interval_map<Ord, Count> _cumulativeFoundUniqueWords;
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
    const auto interval = OrdInterval(ord, ordEnd);
    _cumulativeFoundWordsCnt += std::make_pair(interval, cnt);
    if (!_foundWordsCount.contains(ord)) {
        _cumulativeFoundUniqueWords += std::make_pair(interval, Count{1});
    }
    ++_totalFoundWordsCnt;
    ++_foundWordsCount[ord];
}

}

#endif // BASE_A_D_DICTIONARY_HPP
