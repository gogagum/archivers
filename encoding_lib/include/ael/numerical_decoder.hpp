#ifndef NUMERICAL_DECODER_HPP
#define NUMERICAL_DECODER_HPP

#include <boost/timer/progress_display.hpp>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "dictionary/decreasing_on_update_dictionary.hpp"
#include "arithmetic_decoder.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"

namespace ael {

////////////////////////////////////////////////////////////////////////////////
/// \brief The NumericalDecoder class.
///
class NumericalDecoder {
private:
    struct _CountEntry {
        std::uint64_t ord;
        std::uint64_t cnt;
    };
public:
    struct LayoutInfo {
        std::size_t dictWordsCount;
        std::size_t wordsCntBitsCount;
        std::size_t dictWordsBitsCount;
        std::size_t contentWordsCount;
        std::size_t contentWordsBitsCount;
    };
    using OptOs = std::optional<std::reference_wrapper<std::ostream>>;
public:
    template <std::output_iterator<std::uint64_t> OutIter>
    static void decode(
            auto& source,
            OutIter outIter,
            std::uint64_t maxOrd,
            const LayoutInfo& layoutInfo,
            OptOs os = std::nullopt);
private:

    static std::vector<std::uint64_t> _decodeCounts(
        auto& source,
        const LayoutInfo& layoutInfo,
        OptOs os);

    static std::vector<std::uint64_t> _decodeOrds(
        auto& source,
        std::uint64_t maxOrd,
        const LayoutInfo& layoutInfo,
        OptOs os);

    template <std::output_iterator<std::uint64_t> OutIter>
    static void _decodeContent(
        auto& source,
        OutIter outIter,
        std::uint64_t maxOrd,
        const LayoutInfo& layoutInfo,
        const std::vector<std::uint64_t>& ords,
        const std::vector<std::uint64_t>& counts,
        OptOs os);
};

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter>
void NumericalDecoder::decode(
        auto& source,
        OutIter outIter,
        std::uint64_t maxOrd,
        const LayoutInfo& layoutInfo,
        OptOs os) {
    // Decode dictionary words
    auto ords = _decodeOrds(source, maxOrd, layoutInfo, os);
    
    // Decode counts
    auto counts = _decodeCounts(source, layoutInfo, os);

    // Decode content
    _decodeContent(source, outIter, maxOrd, layoutInfo, ords, counts, os);
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::uint64_t> NumericalDecoder::_decodeOrds(
        auto& source,
        std::uint64_t maxOrd,
        const LayoutInfo& layoutInfo,
        OptOs os) {
    auto dictWordsDictionary = dict::DecreasingOnUpdateDictionary(maxOrd, 1);
    auto progressBar = boost::timer::progress_display(layoutInfo.dictWordsCount);
    auto ords = std::vector<std::uint64_t>();
        ArithmeticDecoder::decode(
            source, dictWordsDictionary,
            std::back_inserter(ords),
            layoutInfo.dictWordsCount,
            layoutInfo.dictWordsBitsCount, 
            [&progressBar](){ ++progressBar; });
    return ords;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::uint64_t> NumericalDecoder::_decodeCounts(
        auto& source,
        const LayoutInfo& layoutInfo,
        OptOs os) {
    auto countsDictionary = dict::DecreasingCountDictionary<std::uint64_t>(
        layoutInfo.contentWordsCount);
    auto progresBar = boost::timer::progress_display(layoutInfo.dictWordsCount);
    auto counts = std::vector<std::uint64_t>();
    ArithmeticDecoder::decode(
                source, countsDictionary,
                std::back_inserter(counts),
                layoutInfo.dictWordsCount,
                layoutInfo.wordsCntBitsCount,
                [&progresBar](){ ++progresBar; });
    return counts;
}

////////////////////////////////////////////////////////////////////////////////
template <std::output_iterator<std::uint64_t> OutIter>
void NumericalDecoder::_decodeContent(
        auto& source,
        OutIter outIter,
        std::uint64_t maxOrd,
        const LayoutInfo& layoutInfo,
        const std::vector<std::uint64_t>& ords,
        const std::vector<std::uint64_t>& counts,
        OptOs os) {
    assert(ords.size() == counts.size());
    auto contentDictInitialCounts = std::vector<_CountEntry>();
    std::transform(ords.begin(), ords.end(), counts.begin(),
                   std::back_inserter(contentDictInitialCounts),
                   [](std::uint64_t wordOrd,
                      std::uint64_t count) -> _CountEntry {
                       return { wordOrd, count };
                   });
    auto progressBar = boost::timer::progress_display(layoutInfo.contentWordsCount);
    auto contentDictionary =
        dict::DecreasingOnUpdateDictionary(maxOrd, contentDictInitialCounts);
    ArithmeticDecoder::decode(source, contentDictionary, outIter,
                              layoutInfo.contentWordsCount,
                              layoutInfo.contentWordsBitsCount,
                              [&progressBar](){ ++progressBar; });
}

}  // ael

#endif  // NUMERICAL_DECODER_HPP
