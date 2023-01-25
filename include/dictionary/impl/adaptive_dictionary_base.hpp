#ifndef ADAPTIVE_DICTIONARY_BASE_HPP
#define ADAPTIVE_DICTIONARY_BASE_HPP

#include <unordered_map>
#include <boost/icl/interval_map.hpp>

namespace ga::dict::impl {

namespace bicl = boost::icl;

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryBase class
///
template <typename OrdT, typename CountT>
class AdaptiveDictionaryBase {
protected:
    using Ord = OrdT;
    using Count = CountT;

protected:

    AdaptiveDictionaryBase(Count initialTotalWordsCount)
        : _totalWordsCnt(initialTotalWordsCount) {}

protected:
    using OrdInterval = typename bicl::interval_map<Ord, Count>::interval_type;

protected:
    bicl::interval_map<Ord, Count> _cumulativeWordCounts;
    std::unordered_map<Ord, Count> _wordCnts;
    Count _totalWordsCnt;
};



}

#endif // ADAPTIVE_DICTIONARY_BASE_HPP
