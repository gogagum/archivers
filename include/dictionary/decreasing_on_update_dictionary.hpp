#ifndef DECREASING_ON_UPDATE_DICTIONARY_HPP
#define DECREASING_ON_UPDATE_DICTIONARY_HPP

#include "word_probability_stats.hpp"
#include "integer_random_access_iterator.hpp"
#include "impl/adaptive_dictionary_base.hpp"

#include <cstdint>
#include <string>
#include <sstream>

#include <boost/range/iterator_range.hpp>
#include <boost/range/irange.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingOnUpdateDictionary class
///
class DecreasingOnUpdateDictionary
        : public impl::AdaptiveDictionaryBase<std::uint64_t, std::uint64_t> {
public:
    using Ord = std::uint64_t;
    using Count = std::uint64_t;
    using ProbabilityStats = WordProbabilityStats<Count>;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The NoSuchWord class
    ///
    class NoSuchWord : public std::runtime_error {
    public:
        NoSuchWord(Ord ord) :
            std::runtime_error(_formWord(ord)) {}
    private:
        std::string _formWord(Ord ord) {
            auto retStream = std::ostringstream();
            retStream << "No such word: " << ord;
            return retStream.str();
        }
    };

public:    

    /**
     * @brief DecreasingOnUpdateDictionary constructor from counts mapping
     * @param probRng - counts mapping.
     */
    template <std::ranges::input_range RangeT>
    DecreasingOnUpdateDictionary(Ord maxOrd, const RangeT& probRng);

    /**
     * @brief DecreasingOnUpdateDictionary - generate uniform with `count for
     * each word.
     * @param count
     */
    DecreasingOnUpdateDictionary(Ord maxOrd, Count count);

    /**
     * @brief getWord - word by cumulatove count.
     * @param cumulativeNumFound - count to search for.
     * @return found word.
     */
    [[nodiscard]] Ord getWordOrd(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats - get stats for a word.
     * @param word - word to get stats for.
     * @return [low, high, total]
     */
    [[nodiscard]] ProbabilityStats getProbabilityStats(Ord ord);

    /**
     * @brief getTotalWordsCount - get total words count.
     * @return total words count in a dictionary.
     */
    [[nodiscard]] Count
    getTotalWordsCnt() const { return this->_totalWordsCnt; }

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    void _updateWordCnt(Ord ord);

private:
    const Ord _maxOrd;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::ranges::input_range RangeT>
DecreasingOnUpdateDictionary::DecreasingOnUpdateDictionary(
        Ord maxOrd,
        const RangeT& countRng
        ) : impl::AdaptiveDictionaryBase<Ord, Count>(maxOrd, 0),
            _maxOrd(maxOrd) {
    for (const auto& [ord, count] : countRng) {
        this->_wordCnts[ord] = count;
        this->_cumulativeWordCounts.update(ord, _maxOrd, count);
        this->_totalWordsCnt += count;
    }
}

}

#endif // DECREASING_TOTAL_COUNT_DICTIONARY_HPP
