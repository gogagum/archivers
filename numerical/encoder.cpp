#include <iostream>
#include <ranges>
#include <boost/program_options.hpp>

#include <ael/arithmetic_coder.hpp>
#include <ael/flow/bytes_word_flow.hpp>
#include <ael/word/uint_word.hpp>
#include <ael/dictionary/decreasing_counts_dictionary.hpp>
#include <ael/dictionary/decreasing_on_update_dictionary.hpp>

#include "../common.hpp"

namespace bpo = boost::program_options;

using ael::fl::BytesWordFlow;
using ael::w::BytesWord;

using CountsDict = ael::dict::DecreasingCountDictionary<std::uint64_t>;
using DictWordsDict = ael::dict::DecreasingOnUpdateDictionary;
using ContentDict = ael::dict::DecreasingOnUpdateDictionary;

using DictWordsFlow = std::vector<BytesWord<1>>;

using ael::ArithmeticCoder;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::string logStreamParam;

    try {
        appOptionsDescr.add_options() (
            "input-file,i",
            bpo::value(&inFileName)->required(),
            "In file name."
        ) (
            "out-filename,o",
            bpo::value(&outFileName)->default_value({}),
            "Out file name."
        ) (
            "log-stream,l",
            bpo::value(&logStreamParam)->default_value("stdout"),
            "Log stream."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
        optout::OptOstreamRef outStream = get_out_stream(logStreamParam);
        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
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
                    countsCoder.encode(counts, dataConstructor, countsDict, outStream);
            dataConstructor.putTToPosition<std::uint64_t>(countsBits, dictWordsCountsBitsPos);
        }

        {
            auto wordsDict = DictWordsDict(256, 1);
            auto dictWordsCoder = ArithmeticCoder();
            auto [_1, dictWordsBits] = dictWordsCoder.encode(dictWordsOrds, dataConstructor, wordsDict, outStream);
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
            auto [_2, contentWordsBits] = contentCoder.encode(wordsOrds, dataConstructor, contentDict, outStream);
            dataConstructor.putTToPosition(contentWordsBits, contentWordsBitsCountPos);
        }

        fileOpener.getOutFileStream().write(dataConstructor.data<char>(), dataConstructor.size());

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

}
