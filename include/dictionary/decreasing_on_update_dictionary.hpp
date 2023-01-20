#ifndef DECREASING_ON_UPDATE_DICTIONARY_HPP
#define DECREASING_ON_UPDATE_DICTIONARY_HPP

#include "word_probability_stats.hpp"

#include <cstdint>

#include <boost/icl/interval_map.hpp>

namespace ga::dict {

namespace bicl = boost::icl;
namespace br = boost::range;

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingOnUpdateDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class DecreasingOnUpdateDictionary {
public:
    using Word = WordT;
    using Ord = typename WordT::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<Count>;
private:

    DecreasingOnUpdateDictionary() = default;

public:
    template <class RangeT>
    static DecreasingOnUpdateDictionary fromWordCountMap(const RangeT& probRng);

    template <class RangeT>
    static DecreasingOnUpdateDictionary fromWordCumulativeCountMap(const RangeT& probRng);

private:
    using OrdInterval = typename bicl::interval_map<Ord, Count>::interval_type;
private:
    bicl::interval_map<Ord, Count> _cumulativeWordCounts;
    std::unordered_map<Ord, Count> _wordsCounts;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class RangeT>
DecreasingOnUpdateDictionary<WordT, CountT>
DecreasingOnUpdateDictionary<WordT, CountT>::fromWordCountMap(const RangeT& countRng) {
    auto ret = DecreasingOnUpdateDictionary<WordT, CountT>();
    for (const auto& [word, count] : countRng) {
        auto ord = Word::ord(word);
        ret._wordCounts[ord] = count;
        auto interval = OrdInterval(ord, WordT::wordsCount);
        ret._cumulativeWordCounts += std::make_pair(interval, Count{1});
    }
}

}

#endif // DECREASING_TOTAL_COUNT_DICTIONARY_HPP
