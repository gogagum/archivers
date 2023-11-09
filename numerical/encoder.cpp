#include <ael/dictionary/decreasing_counts_dictionary.hpp>
#include <ael/dictionary/decreasing_on_update_dictionary.hpp>
#include <ael/numerical_coder.hpp>
#include <applib/file_opener.hpp>
#include <applib/flow/bytes_word_flow.hpp>
#include <applib/log_stream_get.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <applib/progress_bar.hpp>
#include <boost/program_options.hpp>
#include <iostream>

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

    constexpr auto barWidth = 50uz;
    auto wordsProgressBar = ProgressBar(barWidth, countsMapping.size(),
                                        "Encoding words", outStream);
    auto countsProgressBar = ProgressBar(barWidth, countsMapping.size(),
                                         "Encoding counts", outStream);
    auto contentProgressBar =
        ProgressBar(barWidth, ordFlow.size(), "Encoding content", outStream);

    auto [dataCtr2, dictSize, contentWordsEncoded, totalBitsCnt, maxOrd] =
        ael::NumericalCoder(std::move(dataConstructor))
            .encode(ordFlow, countsMapping, wordsProgressBar.getTick(),
                    countsProgressBar.getTick(), contentProgressBar.getTick());

    dataCtr2->putTToPosition(dictSize, dictSizePos);
    dataCtr2->putTToPosition(totalBitsCnt, totalBitsCntPos);

    fileOpener.getOutFileStream().write(dataCtr2->data(), dataCtr2->size());

  } catch (const std::runtime_error& error) {
    std::cerr << error.what() << "\n";
    return 1;
  }
}
