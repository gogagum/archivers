#ifndef PPMA_DICTIONARY_HPP
#define PPMA_DICTIONARY_HPP

#include "ael/dictionary/decreasing_on_update_dictionary.hpp"
#include "ael/dictionary/impl/cumulative_count.hpp"
#include "ael/dictionary/impl/cumulative_unique_count.hpp"
#include "word_probability_stats.hpp"

#include <boost/range/iterator_range.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

namespace ael::dict {

namespace bpm = boost::multiprecision;

////////////////////////////////////////////////////////////////////////////////
/// \brief PPMADictionary - ppma probability model.
///
class PPMADictionary {
public:
    using Ord = std::uint64_t;
    using Count = bpm::uint256_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 240;
private:
    constexpr const static std::uint16_t _maxSeqLenLog2 = 40;
public:

    /**
     * PPMA dictionary constructor.
     * @param maxOrd - maximal order. 
     */
    PPMADictionary(Ord maxOrd, std::size_t ctxLength);

    /**
     * @brief getWordOrd - get word order index by cumulative count.
     * @param cumulativeNumFound search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats - get probability stats and update.
     * @param ord - order of a word.
     * @return [low, high, total]
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount - get total words count estimation.
     * @return total words count estimation
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

private:
    using _SearchCtx = boost::container::static_vector<Ord, 16>;
    using _SearchCtxHash = boost::hash<_SearchCtx>;
    using _CtxCountMapping = std::unordered_map<
        _SearchCtx,
        impl::CumulativeCount,
        _SearchCtxHash
    >;
private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    ProbabilityStats _getProbabilityStats(Ord ord) const;

    void _updateWordCnt(Ord ord, DecreasingOnUpdateDictionary::Count cnt);

    _SearchCtx _getSearchCtxEmptySkipped() const;

private:
    std::size_t _maxOrd;
    impl::CumulativeCount _zeroCtxCnt;
    impl::CumulativeUniqueCount _zeroCtxUniqueCnt;
    std::deque<Ord> _ctx;
    _CtxCountMapping _ctxInfo;
    const std::size_t _ctxLength;
};

}

#endif