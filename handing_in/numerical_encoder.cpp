#include <iostream>
#include <optional>
#include <ranges>

#include <ael/arithmetic_coder.hpp>
#include <ael/flow/bytes_word_flow.hpp>
#include <ael/dictionary/decreasing_counts_dictionary.hpp>
#include <ael/dictionary/decreasing_on_update_dictionary.hpp>

#include <applib/opt_ostream.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <applib/file_opener.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>

using ael::fl::BytesWordFlow;
using ael::w::BytesWord;

using CountsDict = ael::dict::DecreasingCountDictionary<std::uint64_t>;
using DictWordsDict = ael::dict::DecreasingOnUpdateDictionary;
using ContentDict = ael::dict::DecreasingOnUpdateDictionary;

using DictWordsFlow = std::vector<BytesWord<1>>;

using ael::ArithmeticCoder;

int main(int argc, char* argv[]) {
    try {
        if (argc < 2 || argc > 3) {
            std::stringstream message;            
            message << "Usage: " << std::string(argv[0])
                    << " <input_file> [<output_file>]" << std::endl;
            throw std::invalid_argument(message.str());   
        }
        std::string inFileName = argv[1];
        std::string outFileName = (argc == 3) ? argv[2] : inFileName + "-encoded";

        outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
        auto fileOpener = FileOpener(inFileName, outFileName, std::nullopt);
        auto inFileBytes = fileOpener.getInData();
        auto wordFlow = BytesWordFlow<1>(inFileBytes);

        auto countsMap = std::map<BytesWord<1>::Ord, std::uint64_t>();

        for (auto word : wordFlow) {
            ++countsMap[BytesWord<1>::ord(word)];
        }

        std::vector<std::pair<std::uint64_t, std::uint64_t>> countsMapping;

        for (auto [ord, count] : countsMap) {
            countsMapping.emplace_back(ord, count);
        }

        std::sort(countsMapping.begin(), countsMapping.end(),
                  [](const auto& c0, const auto& c1){
                      return c0.second > c1.second;
                  });

        auto dataConstructor = ael::ByteDataConstructor();

        std::vector<std::uint64_t> counts;
        std::vector<std::uint64_t> dictWordsOrds;

        for (auto [ord, count] : countsMapping) {
            counts.push_back(count);
            dictWordsOrds.push_back(ord);
        }

        dataConstructor.putT<std::uint64_t>(counts.size());

        const auto dictWordsCountsBitsPos =
                dataConstructor.saveSpaceForT<std::uint64_t>();
        const auto dictWordsBitsPos =
                dataConstructor.saveSpaceForT<std::uint64_t>();
        const auto contentWordsBitsCountPos =
                dataConstructor.saveSpaceForT<std::uint64_t>();

        dataConstructor.putT<std::uint64_t>(wordFlow.size());

        {
            auto countsDict = CountsDict(wordFlow.size());
            auto countsCoder = ArithmeticCoder();
            auto [_0, countsBits] =
                    countsCoder.encode(counts, dataConstructor, countsDict, std::nullopt);
            dataConstructor.putTToPosition<std::uint64_t>(countsBits, dictWordsCountsBitsPos);
        }

        {
            auto wordsDict = DictWordsDict(256, 1);
            auto dictWordsCoder = ArithmeticCoder();
            auto [_1, dictWordsBits] = dictWordsCoder.encode(dictWordsOrds, dataConstructor, wordsDict, std::nullopt);
            dataConstructor.putTToPosition<std::uint64_t>(dictWordsBits, dictWordsBitsPos);
        }

        {
            auto contentDict = ContentDict(256, countsMapping);
            auto contentCoder = ArithmeticCoder();
            std::vector<std::uint64_t> wordsOrds;
            std::transform(wordFlow.begin(), wordFlow.end(),
                           std::back_inserter(wordsOrds),
                           [](const auto& word) {
                               return BytesWord<1>::ord(word);
                           });
            auto [_2, contentWordsBits] = contentCoder.encode(wordsOrds, dataConstructor, contentDict, std::nullopt);
            dataConstructor.putTToPosition(contentWordsBits, contentWordsBitsCountPos);
        }

        fileOpener.getOutFileStream().write(dataConstructor.data<char>(), dataConstructor.size());

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

}
