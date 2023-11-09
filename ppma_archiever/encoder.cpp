#include <ael/arithmetic_coder.hpp>
#include <ael/dictionary/ppma_dictionary.hpp>
#include <applib/file_opener.hpp>
#include <applib/log_stream_get.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <applib/progress_bar.hpp>
#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <string>

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
  bpo::options_description appOptionsDescr("Console options.");

  std::string inFileName;
  std::string outFileName;
  std::uint16_t numBits;
  std::size_t ctxLen;
  std::string logStreamParam;

  try {
    appOptionsDescr.add_options()(
        "input-file,i", bpo::value(&inFileName)->required(), "In file name.")(
        "out-filename,o", bpo::value(&outFileName)->default_value({}),
        "Out file name.")("bits,b", bpo::value(&numBits)->default_value(16),
                          "Word bits count.")(
        "ctx-length,c", bpo::value(&ctxLen)->default_value(2),
        "Content cells count.")(
        "log-stream,l", bpo::value(&logStreamParam)->default_value("stdout"),
        "Log stream.");

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
    bpo::notify(vm);

    outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
    auto& outStream = LogStreamGet::getLogStream(logStreamParam);
    auto fileOpener = FileOpener(inFileName, outFileName, outStream);
    auto dict = ael::dict::PPMADictionary({1ull << numBits, ctxLen});

    auto [wordsOrds, tail] =
        OrdAndTailSplitter::process(fileOpener.getInData(), numBits);

    auto encoded = std::make_unique<ael::ByteDataConstructor>();
    encoded->putT<std::uint16_t>(numBits);
    encoded->putT<std::uint16_t>(tail.size());
    encoded->putT<std::uint8_t>(ctxLen);
    const auto wordsCountPos = encoded->saveSpaceForT<std::uint64_t>();
    const auto bitsCountPos = encoded->saveSpaceForT<std::uint64_t>();
    constexpr auto barWidth = 50uz;
    auto progressBar =
        ProgressBar(barWidth, wordsOrds.size(), "Encoding words", outStream);

    auto [encoded2, wordsCount, bitsCount] =
        ael::ArithmeticCoder(std::move(encoded))
            .encode(wordsOrds, dict, progressBar.getTick())
            .finalize();
    encoded2->putTToPosition(wordsCount, wordsCountPos);
    encoded2->putTToPosition(bitsCount, bitsCountPos);
    std::ranges::copy(tail, encoded2->getBitBackInserter());
    fileOpener.getOutFileStream().write(encoded2->data(), encoded2->size());
  } catch (const std::exception& error) {
    std::cerr << error.what() << "\n";
    return 1;
  }

  return 0;
}
