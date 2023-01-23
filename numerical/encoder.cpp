#include <iostream>
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

using CountsDictionary = DecreasingCountDictionary<std::uint64_t>;
using DictWordsDictionary = DecreasingOnUpdateDictionary<BytesWord<1>>;
using ContentDictionary = DecreasingOnUpdateDictionary<BytesWord<1>>;
using UIntWordsFlow = std::vector<UIntWord<std::uint64_t>>;
using DictWordsFlow = std::vector<BytesWord<1>>;
using CountsCoder = ga::ArithmeticCoder<UIntWordsFlow,
                                        CountsDictionary,
                                        40>;
using DictWordsCoder = ga::ArithmeticCoder<DictWordsFlow,
                                           DictWordsDictionary,
                                           40>;
using ContentCoder = ga::ArithmeticCoder<BytesWordFlow<BytesWord<1>>,
                                         ContentDictionary,
                                         40>;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;

    try {
        appOptionsDescr.add_options() (
            "input-file,i",
            bpo::value(&inFileName)->required(),
            "In file name."
        ) (
            "out-filename,o",
            bpo::value(&outFileName)->default_value(inFileName + "-out"),
            "Out file name."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        auto fileOpener = FileOpener(inFileName, outFileName);
        auto inFileBytes = fileOpener.getInData();
        auto wordFlow = BytesWordFlow<BytesWord<1>>(inFileBytes);

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

        auto dictWordsCountsBitsPosition
                = dataConstructor.saveBytesSpace(sizeof(std::uint64_t));

        auto dictWordsBitsPosition
                = dataConstructor.saveBytesSpace(sizeof(std::uint64_t));

        auto contentWordsBitsCountPosition
                = dataConstructor.saveBytesSpace(sizeof(std::uint64_t));

        dataConstructor.putT<std::uint64_t>(wordFlow.size());

        {
            auto countsDictionary = CountsDictionary(wordFlow.size());
            auto countsCoder = CountsCoder(countsWords, std::move(countsDictionary));

            auto [_0, countsBits] = countsCoder.encode(dataConstructor);
            dataConstructor.putTToPosition<std::uint64_t>(countsBits, dictWordsCountsBitsPosition);
        }

        {
            auto dictionaryInitialWordsCounts = std::vector<std::pair<BytesWord<1>, std::uint64_t>>();

            for (auto word : dictWords) {
                dictionaryInitialWordsCounts.emplace_back(word, 1);
            }

            auto dictionaryWordsDictionary = DictWordsDictionary(dictionaryInitialWordsCounts);
            auto dictWordsCoder = DictWordsCoder(dictWords, std::move(dictionaryWordsDictionary));

            auto [_1, dictWordsBits] = dictWordsCoder.encode(dataConstructor);
            dataConstructor.putTToPosition<std::uint64_t>(dictWordsBits, dictWordsBitsPosition);
        }

        {
            auto textDictionary = ContentDictionary(counts);
            auto contentCoder = ContentCoder(wordFlow, std::move(textDictionary));

            auto [_2, contentWordsBits] = contentCoder.encode(dataConstructor);

            dataConstructor.putTToPosition(contentWordsBits, contentWordsBitsCountPosition);
        }
    } catch (const std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return 2;
    }

}
