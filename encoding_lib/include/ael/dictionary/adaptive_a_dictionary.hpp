#ifndef ADAPTIVE_A_DICTIONARY_HPP
#define ADAPTIVE_A_DICTIONARY_HPP

#include "ael/dictionary/impl/contextual_dictionary_base_improved.hpp"
#include "word_probability_stats.hpp"
#include "impl/a_d_dictionary_base.hpp"
#include "impl/contextual_dictionary_base.hpp"

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveADictionary class
///
class AdaptiveADictionary : impl::ADDictionaryBase<std::uint64_t> {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
public:

    /**
     * Adaptive <<A>> dictionary constructor.
     * @param maxOrd - maximal order. 
     */
    AdaptiveADictionary(Ord maxOrd);

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief totalWordsCount
     * @return
     */
    [[nodiscard]] Count getTotalWordsCnt() const;

private:

    Count _getLowerCumulativeCnt(Ord ord) const;

    Count _getWordCnt(Ord ord) const;

    ProbabilityStats _getProbabilityStats(Ord ord) const;

private:
    friend class impl::ContextualDictionaryBase<AdaptiveADictionary>;
    friend class impl::ContextualDictionaryBaseImproved<AdaptiveADictionary>;
};

}

#endif // ADAPTIVE_A_DICTIONARY_HPP
