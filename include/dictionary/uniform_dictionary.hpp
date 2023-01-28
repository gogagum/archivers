#ifndef UNIFORM_DICTIONARY_HPP
#define UNIFORM_DICTIONARY_HPP

#include "word_probability_stats.hpp"

#include <cstdint>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The UniformDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class UniformDictionary {
public:

    using Word = WordT;
    using Ord = typename Word::Ord;
    using Count = CountT;
    using ProbabilityStats = WordProbabilityStats<Count>;

public:

    /**
     * @brief getWord - get word by cumulative num found.
     * @param cumulativeNumFound - search key.
     * @return word with exact cumulative number found.
     */
    [[nodiscard]] Word getWord(Count cumulativeNumFound);

    /**
     * @brief getWordProbabilityStats
     * @param word
     * @return [low, high, total]
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(const Word& word);

    /**
     * @brief totalWordsCount
     * @return
     */
    [[nodiscard]] Count
    getTotalWordsCount() const { return WordT::wordsCount; }
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
WordT UniformDictionary<WordT, CountT>::getWord(Count cumulativeNumFound) {
    return WordT::byOrd(cumulativeNumFound);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto UniformDictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = WordT::ord(word);
    return { ord, ord + 1, WordT::wordsCount };
}


}  // namespace ga::dict

#endif // UNIFORM_DICTIONARY_HPP
