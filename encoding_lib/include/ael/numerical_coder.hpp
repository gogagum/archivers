#ifndef NUMERICAL_CODER_HPP
#define NUMWEICAL_CODER_HPP

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <map>
#include <cstdint>
#include "dictionary/decreasing_on_update_dictionary.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"
#include "byte_data_constructor.hpp"
#include "arithmetic_coder.hpp"

namespace ael {

class NumericalCoder {
public:

    struct EncodeRet {
        std::uint64_t dictSize;
        std::uint64_t wordsBitsCnt;
        std::uint64_t wordsCountsBitsCnt;
        std::uint64_t contentWordsEncoded;
        std::uint64_t contentBitsCnt;
    };

public:
    static EncodeRet encode(auto& ordFlow,
                            ByteDataConstructor& dataConstructor,
                            auto& os);
private:
    struct _CountEntry {
        std::uint64_t ord;
        std::uint64_t count;
    };
    struct _BitsCountsPositions {
        std::size_t countsBitsPos;
        std::size_t wordsBitsPos;
        std::size_t contentBitsPos;
    };
private:
    
    static std::vector<_CountEntry> _countWords(const auto& ordFlow);
    
};

////////////////////////////////////////////////////////////////////////////////
auto NumericalCoder::encode(auto& ordFlow,
                            ByteDataConstructor& dataConstructor,
                            auto& os) -> EncodeRet {
    auto countsMapping = _countWords(ordFlow);

    std::vector<std::uint64_t> counts;
    std::vector<std::uint64_t> dictWordsOrds;

    for (auto [ord, count] : countsMapping) {
        counts.push_back(count);
        dictWordsOrds.push_back(ord);
    }
    
    const auto maxOrd = *std::ranges::max_element(ordFlow) + 1;

    // Encode words
    auto wordsDict = dict::DecreasingOnUpdateDictionary(maxOrd, 1);
    auto [dictWordsEncoded, wordsBitsCnt] =
        ArithmeticCoder::encode(dictWordsOrds, dataConstructor, wordsDict, os);
    assert(dictWordsEncoded == countsMapping.size());

    // Encode counts
    auto countsDict =
        dict::DecreasingCountDictionary<std::uint64_t>(ordFlow.size());
    auto [dictWordsCountsEncoded, countsBitsCnt] =
        ArithmeticCoder::encode(counts, dataConstructor, countsDict, os); 
    assert(dictWordsCountsEncoded == countsMapping.size());
    
    // Encode content
    auto contentDict =
        dict::DecreasingOnUpdateDictionary(maxOrd, countsMapping);
    auto [contentWordsEncoded, contentBitsCnt] =
        ArithmeticCoder::encode(ordFlow, dataConstructor, contentDict, os);
    assert(contentWordsEncoded == ordFlow.size());

    return {
        countsMapping.size(),
        wordsBitsCnt,
        countsBitsCnt,
        ordFlow.size(),
        contentBitsCnt
    };
}

////////////////////////////////////////////////////////////////////////////////
auto NumericalCoder::_countWords(
        const auto& ordFlow) -> std::vector<_CountEntry> {
    auto countsMap = std::map<std::uint64_t, std::uint64_t>();
    for (auto ord : ordFlow) {
        ++countsMap[ord];
    }
    std::vector<_CountEntry> ret;
    std::transform(countsMap.begin(), countsMap.end(), std::back_inserter(ret),
                   [](auto entry) {
                       return _CountEntry(entry.first, entry.second);
                   });
    std::sort(ret.begin(), ret.end(),
              [](const auto& c0, const auto& c1){
                  return c0.count > c1.count;
              });
    return ret;
}

}  // namespace ael

#endif // NUMERICAL_CODER_HPP