#ifndef DECREASING_ON_UPDATE_DICTIONARY_HPP
#define DECREASING_ON_UPDATE_DICTIONARY_HPP

#include "word_probability_stats.hpp"
#include "integer_random_access_iterator.hpp"
#include "impl/adaptive_dictionary_base.hpp"

#include <cstdint>

#include <boost/icl/interval_map.hpp>
#include <boost/range/irange.hpp>

namespace ga::dict {

namespace bicl = boost::icl;
namespace br = boost::range;

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingOnUpdateDictionary class
///
template <class WordT, typename CountT = std::uint64_t>
class DecreasingOnUpdateDictionary
        : public impl::AdaptiveDictionaryBase<typename WordT::Ord, CountT> {
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
        NoSuchWord(const Word& word) :
            std::runtime_error(_formMessage(word)) {}
    private:
        std::string _formMessage(const Word& word) {
            std::string ret;
            auto retStream = std::stringstream(ret);
            retStream << "No such word: " << word;
            return ret;
        }
    };

public:    

    /**
     * @brief DecreasingOnUpdateDictionary constructor from counts mapping
     * @param probRng - counts mapping.
     */
    template <std::ranges::input_range RangeT>
    DecreasingOnUpdateDictionary(const RangeT& probRng);

    /**
     * @brief DecreasingOnUpdateDictionary - generate uniform with `count for
     * each word.
     * @param count
     */
    DecreasingOnUpdateDictionary(Count count);

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
    ProbabilityStats getProbabilityStats(const Word& word);

    /**
     * @brief getTotalWordsCount - get total words count.
     * @return total words count in a dictionary.
     */
    Count getTotalWordsCnt() const { return this->_totalWordsCnt; };

private:

    using OrdInterval =
        typename impl::AdaptiveDictionaryBase<Ord, Count>::OrdInterval;

private:

    Count _getLowerCumulativeNumFound(Ord ord) const;

    void _updateWordCnt(Ord ord);

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
template <std::ranges::input_range RangeT>
DecreasingOnUpdateDictionary<WordT, CountT>::DecreasingOnUpdateDictionary(
        const RangeT& countRng) : impl::AdaptiveDictionaryBase<Ord, Count>(0) {
    for (const auto& [word, count] : countRng) {
        const auto ord = Word::ord(word);
        this->_wordCnts[ord] = count;
        const auto interval = OrdInterval(ord, WordT::wordsCount);
        this->_cumulativeWordCounts += std::make_pair(interval, count);
        this->_totalWordsCnt += count;
    }
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
DecreasingOnUpdateDictionary<WordT, CountT>::DecreasingOnUpdateDictionary(
        Count count)
    : impl::AdaptiveDictionaryBase<Ord, Count>(WordT::wordsCount * count) {
    for (auto ord : boost::irange<typename WordT::Ord>(0, WordT::wordsCount)) {
        this->_wordCnts[ord] = count;
        auto interval = OrdInterval(ord, WordT::wordsCount);
        this->_cumulativeWordCounts += std::make_pair(interval, count);
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
DecreasingOnUpdateDictionary<WordT, CountT>::getProbabilityStats(
        const Word& word) -> ProbabilityStats {
    auto ord = Word::ord(word);
    if (!this->_wordCnts.contains(ord)
            || this->_wordCnts.at(ord) == Count(0)) {
        throw NoSuchWord(word);
    }
    const auto low = _getLowerCumulativeNumFound(ord);
    const auto high = low + this->_wordCnts[ord];
    const auto total = getTotalWordsCnt();
    _updateWordCnt(ord);
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
    return this->_cumulativeWordCounts(ord - 1);
}

//----------------------------------------------------------------------------//
template <class WordT, typename CountT>
void DecreasingOnUpdateDictionary<WordT, CountT>::_updateWordCnt(Ord ord) {
    this->_totalWordsCnt -= 1;
    const auto interval = OrdInterval(ord, WordT::wordsCount);
    this->_cumulativeWordCounts -= std::make_pair(interval, Count{1});
    --this->_wordCnts[ord];
}

}

#endif // DECREASING_TOTAL_COUNT_DICTIONARY_HPP
