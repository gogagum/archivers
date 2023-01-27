#include <iostream>
#include <boost/program_options.hpp>

#include "../common.hpp"
#include "data_parser.hpp"
#include "byte_data_constructor.hpp"
#include "word/uint_word.hpp"
#include "word/bytes_word.hpp"
#include "dictionary/decreasing_counts_dictionary.hpp"
#include "dictionary/decreasing_on_update_dictionary.hpp"
#include "arithmetic_decoder.hpp"

namespace bpo = boost::program_options;

using ga::dict::DecreasingCountDictionary;
using ga::w::BytesWord;
using ga::w::UIntWord;
using ga::dict::DecreasingCountDictionary;
using ga::dict::DecreasingOnUpdateDictionary;

using ga::ArithmeticDecoder;

using CountsDictionary = DecreasingCountDictionary<std::uint64_t>;
using DictWordsDictionary = DecreasingOnUpdateDictionary<BytesWord<1>>;
using ContentDictionary = DecreasingOnUpdateDictionary<BytesWord<1>>;

struct CountMappingRow {
    BytesWord<1> word;
    std::uint64_t count;
};

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

        auto filesOpener = FileOpener(inFileName, outFileName);
        auto decoded = ga::DataParser(filesOpener.getInData());

        const auto dictWordsCount = decoded.takeT<std::uint64_t>();
        std::cerr << "Dictionary size: " << dictWordsCount << std::endl;

        const auto wordsCountsBitsNumber = decoded.takeT<std::uint64_t>();
        std::cerr << "Bits for words counts decoding: "
                  << wordsCountsBitsNumber << std::endl;

        const auto dictWordsBitsNumber = decoded.takeT<std::uint64_t>();
        std::cerr << "Bits for dictionary words decoding: "
                  << dictWordsBitsNumber << std::endl;

        const auto contentBitsNumber = decoded.takeT<std::uint64_t>();
        std::cerr << "Content bits number: "
                  << contentBitsNumber << std::endl;

        const auto contentWordsNumber = decoded.takeT<std::uint64_t>();
        std::cerr << "Content words number: "
                  << contentWordsNumber << std::endl;

        ////////////////////////////////////////////////////////////////////////

        auto countsDictionary = CountsDictionary(contentWordsNumber);
        auto countsWords = std::vector<UIntWord<std::uint64_t>>();
        auto countsDecoder = ArithmeticDecoder();
        countsDecoder.decode(decoded, countsDictionary,
                             std::back_inserter(countsWords),
                             dictWordsCount, wordsCountsBitsNumber, std::cerr);

        ////////////////////////////////////////////////////////////////////////

        auto dictWordsDictionary = DictWordsDictionary(1);
        auto words = std::vector<BytesWord<1>>();
        auto wordsDecoder = ArithmeticDecoder();
        wordsDecoder.decode(decoded, dictWordsDictionary,
                            std::back_inserter(words),
                            dictWordsCount, dictWordsBitsNumber, std::cerr);

        ////////////////////////////////////////////////////////////////////////

        auto contentDictInitialCounts = std::vector<CountMappingRow>();
        std::transform(words.begin(), words.end(), countsWords.begin(),
                       std::back_inserter(contentDictInitialCounts),
                       [](const auto& word, const auto& countWord) -> CountMappingRow {
                           return { word, countWord.getValue() };
                       });

        auto contentDictionary = ContentDictionary(contentDictInitialCounts);

        auto contentWords = std::vector<BytesWord<1>>();
        auto contentDecoder = ArithmeticDecoder();
        contentDecoder.decode(decoded, contentDictionary,
                              std::back_inserter(contentWords),
                              contentWordsNumber, contentBitsNumber, std::cerr);

        ////////////////////////////////////////////////////////////////////////

        auto dataConstructor = ga::ByteDataConstructor();

        for (auto& word: contentWords) {
            word.bitsOut(dataConstructor.getBitBackInserter());
        }

        filesOpener.getOutFileStream().write(
                    dataConstructor.data<char>(), dataConstructor.size());

    } catch (const std::exception&  error) {
        std::cout << error.what();
        return 1;
    }

    return 0;
}
