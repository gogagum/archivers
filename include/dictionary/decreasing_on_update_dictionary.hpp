#ifndef DECREASING_ON_UPDATE_DICTIONARY_HPP
#define DECREASING_ON_UPDATE_DICTIONARY_HPP

#include "word_probability_stats.hpp"
#include "integer_random_access_iterator.hpp"
#include "impl/adaptive_dictionary_base.hpp"

#include <cstdint>

#include <boost/range/iterator_range.hpp>
#include <boost/range/irange.hpp>

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecreasingOnUpdateDictionary class
///
template <std::integral OrdT = std::uint64_t>
class DecreasingOnUpdateDictionary
        : public impl::AdaptiveDictionaryBase<OrdT, std::uint64_t> {
public:
    using Ord = OrdT;
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
            std::string ret;
            auto retStream = std::stringstream(ret);
            retStream << "No such word: " << ord;
            return ret;
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
template <std::integral OrdT>
template <std::ranges::input_range RangeT>
DecreasingOnUpdateDictionary<OrdT>::DecreasingOnUpdateDictionary(
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

//----------------------------------------------------------------------------//
template <std::integral OrdT>
DecreasingOnUpdateDictionary<OrdT>::DecreasingOnUpdateDictionary(
        Ord maxOrd,
        Count count
        ) : impl::AdaptiveDictionaryBase<Ord, Count>(_maxOrd, _maxOrd * count),
            _maxOrd(maxOrd) {
    for (auto ord : boost::irange<Ord>(0, _maxOrd)) {
        this->_wordCnts[ord] = count;
        this->_cumulativeWordCounts.update(ord, _maxOrd, count);
    }
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto DecreasingOnUpdateDictionary<OrdT>::getWordOrd(
        Count cumulativeNumFound) const -> Ord {
    using UintIt = misc::IntegerRandomAccessIterator<std::uint64_t>;
    auto idxs = boost::make_iterator_range<UintIt>(0, _maxOrd);
    // TODO: replace
    //auto idxs = std::ranges::iota_view(std::uint64_t{0}, WordT::wordsCount);
    const auto getLowerCumulNumFound_ = [this](Ord ord) {
        return this->_getLowerCumulativeNumFound(ord + 1);
    };
    auto it = std::ranges::upper_bound(idxs, cumulativeNumFound, {},
                                       getLowerCumulNumFound_);
    return it - idxs.begin();
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto DecreasingOnUpdateDictionary<OrdT>::getProbabilityStats(
        Ord ord) -> ProbabilityStats {
    if (!this->_wordCnts.contains(ord)
            || this->_wordCnts.at(ord) == Count(0)) {
        throw NoSuchWord(ord);
    }
    const auto low = _getLowerCumulativeNumFound(ord);
    const auto high = low + this->_wordCnts[ord];
    const auto total = getTotalWordsCnt();
    _updateWordCnt(ord);
    return { low, high, total };
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
auto DecreasingOnUpdateDictionary<OrdT>::_getLowerCumulativeNumFound(
        Ord ord) const -> Count {
    if (ord == 0) {
        return Count{0};
    }
    return this->_cumulativeWordCounts.get(ord - 1);
}

//----------------------------------------------------------------------------//
template <std::integral OrdT>
void DecreasingOnUpdateDictionary<OrdT>::_updateWordCnt(Ord ord) {
    this->_totalWordsCnt -= 1;
    this->_cumulativeWordCounts.update(ord, _maxOrd, - 1);;
    --this->_wordCnts[ord];
}

}

#endif // DECREASING_TOTAL_COUNT_DICTIONARY_HPP
