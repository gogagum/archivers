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

using CountsDictionary = DecreasingCountDictionary<std::uint64_t>;
using DictWordsDictionary = DecreasingOnUpdateDictionary<BytesWord<1>>;
using ContentDictionary = DecreasingOnUpdateDictionary<BytesWord<1>>;

using DictWordsDecoder = ga::ArithmeticDecoder<DictWordsDictionary>;
using CountsDecoder = ga::ArithmeticDecoder<CountsDictionary>;
using ContentDecoder = ga::ArithmeticDecoder<ContentDictionary>;

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
        auto countsDecoder = CountsDecoder(std::move(countsDictionary));

        auto counts = countsDecoder.decode(decoded, dictWordsCount, wordsCountsBitsNumber);

        ////////////////////////////////////////////////////////////////////////

        auto dictWordsDictionary = DictWordsDictionary(1);
        auto dictWordsDecoder = DictWordsDecoder(std::move(dictWordsDictionary));

        auto words = dictWordsDecoder.decode(
                    decoded, dictWordsCount, dictWordsBitsNumber);

        ////////////////////////////////////////////////////////////////////////

        auto contentDictInitialCounts = std::vector<CountMappingRow>();
        std::transform(words.begin(), words.end(), counts.begin(),
                       std::back_inserter(contentDictInitialCounts),
                       [](const auto& word, const auto& countWord) -> CountMappingRow {
                           return { word, countWord.getValue() };
                       });

        auto contentDictionary = ContentDictionary(contentDictInitialCounts);
        auto contentDecoder = ContentDecoder(std::move(contentDictionary));

        auto contentWords = contentDecoder.decode(
                    decoded, contentWordsNumber, contentBitsNumber);

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
