#include <iostream>
#include <boost/program_options.hpp>

#include <ael/data_parser.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/word/bytes_word.hpp>
#include <ael/dictionary/decreasing_counts_dictionary.hpp>
#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <ael/arithmetic_decoder.hpp>

#include <applib/opt_ostream.hpp>
#include <applib/file_opener.hpp>
#include <applib/decode_impl.hpp>
#include <optional>

using ael::w::BytesWord;

using ael::ArithmeticDecoder;

using CountsDictionary = ael::dict::DecreasingCountDictionary<std::uint64_t>;
using DictWordsDictionary = ael::dict::DecreasingOnUpdateDictionary;
using ContentDictionary = ael::dict::DecreasingOnUpdateDictionary;

struct CountMappingRow {
    std::uint64_t word;
    std::uint64_t count;
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 2 || argc > 3) {
            std::stringstream message;            
            message << "Usage: " << std::string(argv[0])
                    << " <input_file> [<output_file>]" << std::endl;
            throw std::invalid_argument(message.str());   
        }
        std::string inFileName = argv[1];
        std::string outFileName = (argc == 3) ? argv[2] : inFileName + "-decoded";

        auto filesOpener = FileOpener(inFileName, outFileName, std::nullopt);
        auto decoded = ael::DataParser(filesOpener.getInData());

        const auto dictWordsCount = decoded.takeT<std::uint64_t>();
        const auto wordsCountsBitsNumber = decoded.takeT<std::uint64_t>();
        const auto dictWordsBitsNumber = decoded.takeT<std::uint64_t>();
        const auto contentBitsNumber = decoded.takeT<std::uint64_t>();
        const auto contentWordsNumber = decoded.takeT<std::uint64_t>();

        ////////////////////////////////////////////////////////////////////////

        auto countsDictionary = CountsDictionary(contentWordsNumber);
        auto counts = std::vector<std::uint64_t>();
        auto countsDecoder = ArithmeticDecoder();
        countsDecoder.decode(
                    decoded, countsDictionary,
                    std::back_inserter(counts),
                    dictWordsCount, wordsCountsBitsNumber, std::nullopt);

        ////////////////////////////////////////////////////////////////////////

        auto dictWordsDictionary = DictWordsDictionary(256, 1);
        auto wordsOrds = std::vector<std::uint64_t>();
        auto wordsDecoder = ArithmeticDecoder();
        wordsDecoder.decode(
                    decoded, dictWordsDictionary,
                    std::back_inserter(wordsOrds),
                    dictWordsCount, dictWordsBitsNumber, std::nullopt);

        ////////////////////////////////////////////////////////////////////////

        auto contentDictInitialCounts = std::vector<CountMappingRow>();
        std::transform(wordsOrds.begin(), wordsOrds.end(), counts.begin(),
                       std::back_inserter(contentDictInitialCounts),
                       [](std::uint64_t wordOrd, std::uint64_t count) -> CountMappingRow {
                           return { wordOrd, count };
                       });

        auto contentDictionary = ContentDictionary(256, contentDictInitialCounts);

        auto contentWordsOrds = std::vector<std::uint64_t>();
        auto contentDecoder = ArithmeticDecoder();
        contentDecoder.decode(
                    decoded, contentDictionary,
                    std::back_inserter(contentWordsOrds),
                    contentWordsNumber, contentBitsNumber, std::nullopt);

        ////////////////////////////////////////////////////////////////////////

        auto dataConstructor = ael::ByteDataConstructor();

        for (auto& wordOrd: contentWordsOrds) {
            auto word = ael::w::BytesWord<1>::byOrd(wordOrd);
            word.bytesOut(dataConstructor.getByteBackInserter());
        }

        filesOpener.getOutFileStream().write(
                    dataConstructor.data<char>(), dataConstructor.size());

    } catch (const std::exception&  error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
