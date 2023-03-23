#include <cstdint>
#include <iostream>
#include <iterator>

#include <boost/program_options.hpp>

#include <indicators/progress_bar.hpp>

#include <ael/numerical_decoder.hpp>
#include <ael/data_parser.hpp>
#include <ael/byte_data_constructor.hpp>
#include <ael/word/bytes_word.hpp>

#include <applib/file_opener.hpp>
#include <applib/decode_impl.hpp>

using ael::w::BytesWord;

int main(int argc, char* argv[]) {
    try {
        auto cfg =
            DecodeImpl::configure(argc, argv);

        const auto takeWithLog = [&]<class T>(const std::string& msg, T) {    
            const auto ret = cfg.decoded.takeT<T>();
            cfg.outStream << msg << static_cast<std::int64_t>(ret) << std::endl;
            return ret;
        };

        const auto dictSize =
            takeWithLog("Dictionary size: ", std::uint64_t{});
        const auto wordsBitsCnt =
            takeWithLog("Bits count for dictionary words decoding: ", std::uint64_t{});
        const auto wordsCountsBitsCnt =
            takeWithLog("Bits count for words counts decoding: ", std::uint64_t{});
        const auto contentWordsCnt =
            takeWithLog("Content words number: ", std::uint64_t{});
        const auto contentBitsCnt =
            takeWithLog("Bits for content decoding: ", std::uint64_t{});

        auto contentWordsOrds = std::vector<std::uint64_t>();
        const auto layoutInfo = ael::NumericalDecoder::LayoutInfo {
            dictSize, wordsCountsBitsCnt, wordsBitsCnt, contentWordsCnt, contentBitsCnt
        };
        auto wordsProgressBar = indicators::ProgressBar(
            indicators::option::BarWidth{50},
            indicators::option::MaxProgress{layoutInfo.dictWordsCount},
            indicators::option::ShowPercentage{true},
            indicators::option::PostfixText{"Decoding words"},
            indicators::option::Stream{cfg.outStream}
        );
        auto countsProgressBar = indicators::ProgressBar(
            indicators::option::BarWidth{50},
            indicators::option::MaxProgress{layoutInfo.dictWordsCount},
            indicators::option::ShowPercentage{true},
            indicators::option::PostfixText{"Decoding counts"},
            indicators::option::Stream{cfg.outStream}
        );
        auto contentProgressBar = indicators::ProgressBar(
            indicators::option::BarWidth{50},
            indicators::option::MaxProgress{layoutInfo.contentWordsCount},
            indicators::option::ShowPercentage{true},
            indicators::option::PostfixText{"Decoding content"},
            indicators::option::Stream{cfg.outStream}
        );
        ael::NumericalDecoder::decode(
            cfg.decoded, std::back_inserter(contentWordsOrds), 256,
            layoutInfo,
            [&wordsProgressBar]{ wordsProgressBar.tick(); },
            [&countsProgressBar]{ countsProgressBar.tick(); },
            [&contentProgressBar]{ contentProgressBar.tick(); });

        auto dataConstructor = ael::ByteDataConstructor();

        for (auto& wordOrd: contentWordsOrds) {
            auto word = BytesWord<1>::byOrd(wordOrd);
            word.bytesOut(dataConstructor.getByteBackInserter());
        }

        cfg.fileOpener.getOutFileStream().write(
                    dataConstructor.data<char>(), dataConstructor.size());

    } catch (const std::exception&  error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
