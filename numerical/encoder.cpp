#include <iostream>
#include <ranges>
#include <boost/program_options.hpp>

#include "arithmetic_coder.hpp"
#include "flow/bytes_word_flow.hpp"
#include "word/uint_word.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"
#include "dictionary/decreasing_on_update_dictionary.hpp"

#include "../common.hpp"

namespace bpo = boost::program_options;

using ga::fl::BytesWordFlow;
using ga::w::BytesWord;
using ga::w::UIntWord;
using ga::dict::DecreasingCountDictionary;
using ga::dict::DecreasingOnUpdateDictionary;

using CountsDict = DecreasingCountDictionary<std::uint64_t>;
using DictWordsDict = DecreasingOnUpdateDictionary<BytesWord<1>>;
using ContentDict = DecreasingOnUpdateDictionary<BytesWord<1>>;

using UIntWordsFlow = std::vector<UIntWord<std::uint64_t>>;
using DictWordsFlow = std::vector<BytesWord<1>>;

using CountsCoder = ga::ArithmeticCoder<UIntWordsFlow>;
using DictWordsCoder = ga::ArithmeticCoder<DictWordsFlow>;
using ContentCoder = ga::ArithmeticCoder<BytesWordFlow<1>>;

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
            bpo::value(&outFileName)->default_value(inFileName + "-out"),
            "Out file name."
        ) (
            "log-stream,l",
            bpo::value(&logStreamParam)->default_value("stdout"),
            "Log stream."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        optout::OptOstreamRef outStream;

        if (logStreamParam == "stdout") {
            outStream = std::cout;
        } else if (logStreamParam == "stderr") {
            outStream = std::cerr;
        } else if (logStreamParam == "off") {
        } else {
            throw InvalidStreamParam(logStreamParam);
        }

        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
        auto inFileBytes = fileOpener.getInData();
        auto wordFlow = BytesWordFlow<1>(inFileBytes);

        auto countsMap = std::map<BytesWord<1>::Ord, std::uint64_t>();

        for (auto word : wordFlow) {
            ++countsMap[BytesWord<1>::ord(word)];
        }

        std::vector<std::pair<BytesWord<1>, std::uint64_t>> counts;

        for (auto [ord, count] : countsMap) {
            counts.emplace_back(BytesWord<1>::byOrd(ord), count);
        }

        std::sort(counts.begin(), counts.end(),
                  [](const auto& c0, const auto& c1){
                      return c0.second > c1.second;
                  });

        auto dataConstructor = ga::ByteDataConstructor();


        UIntWordsFlow countsWords;
        DictWordsFlow dictWords;

        for (auto [word, count] : counts) {
            countsWords.emplace_back(count);
            dictWords.push_back(word);
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
            auto countsCoder = CountsCoder(countsWords);
            auto [_0, countsBits] =
                    countsCoder.encode(dataConstructor, countsDict, outStream);
            dataConstructor.putTToPosition<std::uint64_t>(countsBits, dictWordsCountsBitsPos);
        }

        {
            auto wordsDict = DictWordsDict(1);
            auto dictWordsCoder = DictWordsCoder(dictWords);
            auto [_1, dictWordsBits] = dictWordsCoder.encode(dataConstructor, wordsDict, outStream);
            dataConstructor.putTToPosition<std::uint64_t>(dictWordsBits, dictWordsBitsPos);
        }

        {
            auto contentDict = ContentDict(counts);
            auto contentCoder = ContentCoder(wordFlow);
            auto [_2, contentWordsBits] = contentCoder.encode(dataConstructor, contentDict, outStream);
            dataConstructor.putTToPosition(contentWordsBits, contentWordsBitsCountPos);
        }

        fileOpener.getOutFileStream().write(dataConstructor.data<char>(), dataConstructor.size());

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 2;
    }

}
