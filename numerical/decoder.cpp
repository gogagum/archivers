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
        auto [outStream, filesOpener, decoded] =
            DecodeImpl::configure(argc, argv);

        const auto dictWordsCount = decoded.takeT<std::uint64_t>();
        outStream << "Dictionary size: " << dictWordsCount << std::endl;

        const auto wordsCountsBitsNumber = decoded.takeT<std::uint64_t>();
        outStream << "Bits for words counts decoding: "
                  << wordsCountsBitsNumber << std::endl;

        const auto dictWordsBitsNumber = decoded.takeT<std::uint64_t>();
        outStream << "Bits for dictionary words decoding: "
                  << dictWordsBitsNumber << std::endl;

        const auto contentBitsNumber = decoded.takeT<std::uint64_t>();
        outStream << "Content bits number: "
                  << contentBitsNumber << std::endl;

        const auto contentWordsNumber = decoded.takeT<std::uint64_t>();
        outStream << "Content words number: "
                  << contentWordsNumber << std::endl;

        ////////////////////////////////////////////////////////////////////////

        auto countsDictionary = CountsDictionary(contentWordsNumber);
        auto counts = std::vector<std::uint64_t>();
        ArithmeticDecoder::decode(
                    decoded, countsDictionary,
                    std::back_inserter(counts),
                    dictWordsCount, wordsCountsBitsNumber, outStream);

        ////////////////////////////////////////////////////////////////////////

        auto dictWordsDictionary = DictWordsDictionary(256, 1);
        auto wordsOrds = std::vector<std::uint64_t>();
        ArithmeticDecoder::decode(
                    decoded, dictWordsDictionary,
                    std::back_inserter(wordsOrds),
                    dictWordsCount, dictWordsBitsNumber, outStream);

        ////////////////////////////////////////////////////////////////////////

        auto contentDictInitialCounts = std::vector<CountMappingRow>();
        std::transform(wordsOrds.begin(), wordsOrds.end(), counts.begin(),
                       std::back_inserter(contentDictInitialCounts),
                       [](std::uint64_t wordOrd, std::uint64_t count) -> CountMappingRow {
                           return { wordOrd, count };
                       });

        auto contentDictionary = ContentDictionary(256, contentDictInitialCounts);

        auto contentWordsOrds = std::vector<std::uint64_t>();
        ArithmeticDecoder::decode(
                    decoded, contentDictionary,
                    std::back_inserter(contentWordsOrds),
                    contentWordsNumber, contentBitsNumber, outStream);

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
