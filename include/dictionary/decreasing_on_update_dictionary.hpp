#ifndef DECREASING_ON_UPDATE_DICTIONARY_HPP
#define DECREASING_ON_UPDATE_DICTIONARY_HPP

#include "word_probability_stats.hpp"
#include "integer_random_access_iterator.hpp"

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

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The NoSuchWord class
    ///
    class NoSuchWord : public std::runtime_error {
    public:
        NoSuchWord() : std::runtime_error("No such word.") {}  // TODO: message with a word
    };

public:

    /**
     * @brief DecreasingOnUpdateDictionary constructor from counts mapping
     * @param probRng - counts mapping.
     */
    template <class RangeT>
    DecreasingOnUpdateDictionary(const RangeT& probRng);

    /**
     * @brief getWord - word by cumulatove count.
     * @param cumulativeNumFound - count to search for.
     * @return found word.
     */
    Word getWord(Count cumulativeNumFound) const;

    /**
     * @brief getWordProbabilityStats - get stats for a word.
     * @param word - word to get stats for.
     * @return [low, high, total]
     */
    ProbabilityStats getWordProbabilityStats(const Word& word);

    /**
     * @brief getTotalWordsCount - get total words count.
     * @return total words count in a dictionary.
     */
    Count getTotalWordsCount() const { return _totalWordsCount; };

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    void _updateWordCount(Ord ord);

private:
    using OrdInterval = typename bicl::interval_map<Ord, Count>::interval_type;
private:
    bicl::interval_map<Ord, Count> _cumulativeWordCounts;
    std::unordered_map<Ord, Count> _wordsCounts;
    Count _totalWordsCount;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <class RangeT>
DecreasingOnUpdateDictionary<WordT, CountT>::DecreasingOnUpdateDictionary(
        const RangeT& countRng) : _totalWordsCount(0) {
    for (const auto& [word, count] : countRng) {
        auto ord = Word::ord(word);
        _wordsCounts[ord] = count;
        auto interval = OrdInterval(ord, WordT::wordsCount);
        _cumulativeWordCounts += std::make_pair(interval, count);
        _totalWordsCount += count;
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto DecreasingOnUpdateDictionary<WordT, CountT>::getWord(
        Count cumulativeNumFound) const -> Word {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, WordT::wordsCount);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeNumFound(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return Word::byOrd(it - idxs.begin());
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
DecreasingOnUpdateDictionary<WordT, CountT>::getWordProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    if (!_wordsCounts.contains(ord) || _wordsCounts.at(ord) == Count(0)) {
        throw NoSuchWord();
    }
    auto low = _getLowerCumulativeNumFound(ord);
    auto high = low + _wordsCounts[ord];
    auto total = getTotalWordsCount();
    _updateWordCount(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
auto
DecreasingOnUpdateDictionary<WordT, CountT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (ord == 0) {
        return Count{0};
    }
    return _cumulativeWordCounts(ord - 1);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void DecreasingOnUpdateDictionary<WordT, CountT>::_updateWordCount(Ord ord) {
    _totalWordsCount -= 1;
    auto interval = OrdInterval(ord, WordT::wordsCount);
    _cumulativeWordCounts -= std::make_pair(interval, Count{1});
    --_wordsCounts[ord];
}

}

#endif // DECREASING_TOTAL_COUNT_DICTIONARY_HPP