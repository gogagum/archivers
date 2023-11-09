#include <ael/dictionary/decreasing_counts_dictionary.hpp>
#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <ael/numerical_coder.hpp>
#include <applib/file_opener.hpp>
#include <applib/flow/bytes_word_flow.hpp>
#include <applib/log_stream_get.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <iterator>
#include <ranges>

namespace bpo = boost::program_options;

using CountsDict = ael::dict::DecreasingCountDictionary<std::uint64_t>;
using DictWordsDict = ael::dict::DecreasingOnUpdateDictionary;
using ContentDict = ael::dict::DecreasingOnUpdateDictionary;

int main(int argc, char* argv[]) {
  bpo::options_description appOptionsDescr("Console options.");

  std::string inFileName;
  std::string outFileName;
  std::string logStreamParam;

  try {
    appOptionsDescr.add_options()(
        "input-file,i", bpo::value(&inFileName)->required(), "In file name.")(
        "out-filename,o", bpo::value(&outFileName)->default_value({}),
        "Out file name.")("log-stream,l",
                          bpo::value(&logStreamParam)->default_value("stdout"),
                          "Log stream.");

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
    bpo::notify(vm);

    outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
    auto& outStream = LogStreamGet::getLogStream(logStreamParam);
    auto fileOpener = FileOpener(inFileName, outFileName, outStream);
    auto inFileBytes = fileOpener.getInData();
    auto ordFlow = std::vector<std::uint64_t>();

    auto dataConstructor = std::make_unique<ael::ByteDataConstructor>();

    const auto dictSizePos = dataConstructor->saveSpaceForT<std::uint64_t>();
    const auto totalBitsCntPos =
        dataConstructor->saveSpaceForT<std::uint64_t>();
    dataConstructor->putT<std::uint64_t>(ordFlow.size());
    const auto contentBitsCntPos =
        dataConstructor->saveSpaceForT<std::uint64_t>();

    auto countsMapping = ael::NumericalCoder::countWords(ordFlow);

    auto wordsProgressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{countsMapping.size()},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Encoding words"},
        indicators::option::Stream{outStream});
    auto countsProgressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{countsMapping.size()},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Encoding counts"},
        indicators::option::Stream{outStream});
    auto contentProgressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{ordFlow.size()},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Encoding content"},
        indicators::option::Stream{outStream});

    auto [dataCtr2, dictSize, contentWordsEncoded, totalBitsCnt, maxOrd] =
        ael::NumericalCoder(std::move(dataConstructor))
            .encode(
                ordFlow, countsMapping,
                [&wordsProgressBar] {
                  wordsProgressBar.tick();
                },
                [&countsProgressBar] {
                  countsProgressBar.tick();
                },
                [&contentProgressBar] {
                  contentProgressBar.tick();
                });

    dataCtr2->putTToPosition(dictSize, dictSizePos);
    dataCtr2->putTToPosition(totalBitsCnt, totalBitsCntPos);

    fileOpener.getOutFileStream().write(reinterpret_cast<const char*>(dataCtr2->data()),
                                        dataCtr2->size());

  } catch (const std::runtime_error& error) {
    std::cerr << error.what() << "\n";
    return 1;
  }
}
