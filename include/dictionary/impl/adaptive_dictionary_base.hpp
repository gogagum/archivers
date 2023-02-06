#ifndef ADAPTIVE_DICTIONARY_BASE_HPP
#define ADAPTIVE_DICTIONARY_BASE_HPP

#include <unordered_map>
#include <dst/dynamic_segment_tree.hpp>

namespace ga::dict::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryBase class
///
template <typename OrdT, typename CountT>
class AdaptiveDictionaryBase {
protected:
    using Ord = OrdT;
    using Count = CountT;

protected:

    AdaptiveDictionaryBase(OrdT wordsCount, Count initialTotalWordsCount)
        : _cumulativeWordCounts(OrdT{0}, wordsCount, 0),
          _totalWordsCnt(initialTotalWordsCount) {}

protected:
    using DST =
        dst::DynamicSegmentTree<
            Ord, Count, void, dst::NoRangeGetOp, dst::NoRangeGetOp,
            std::plus<void>, std::int64_t>;


protected:
    DST _cumulativeWordCounts;
    std::unordered_map<Ord, Count> _wordCnts;
    Count _totalWordsCnt;
};



}

#endif // ADAPTIVE_DICTIONARY_BASE_HPP
