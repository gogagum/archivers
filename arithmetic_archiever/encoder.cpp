#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include <ael/arithmetic_coder.hpp>
#include <ael/dictionary/adaptive_dictionary.hpp>
#include "../common.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;
    std::uint64_t ratio;
    std::string logStreamParam;

    try {
        appOptionsDescr.add_options() (
                "input-file,i",
                bpo::value(&inFileName)->required(),
                "In file name."
            ) (
                "out-filename,o",
                bpo::value(&outFileName)->default_value({}),
                "Out file name."
            ) (
                "bits,b",
                bpo::value(&numBits)->default_value(16),
                "Word bits count."
            ) (
                "ratio,r",
                bpo::value(&ratio)->default_value(2),
                "Dictionary ratio."
            ) (
                "log-stream,l",
                bpo::value(&logStreamParam)->default_value("stdout"),
                "Log stream."
            );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
        optout::OptOstreamRef outStream = get_out_stream(logStreamParam);
        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
        auto dict = ael::dict::AdaptiveDictionary(1ull << numBits, ratio);

        auto [wordsOrds, tail] = OrdAndTailSplitter::process(fileOpener.getInData(), numBits);

        auto coder = ael::ArithmeticCoder();
        auto encoded = ael::ByteDataConstructor();
        encoded.putT<std::uint16_t>(numBits);
        encoded.putT<std::uint16_t>(tail.size());
        const auto wordsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        const auto bitsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        auto [wordsCount, bitsCount] = coder.encode(wordsOrds, encoded, dict, outStream);
        encoded.putTToPosition(wordsCount, wordsCountPos);
        encoded.putTToPosition(bitsCount, bitsCountPos);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 2;
    }

    return 0;
}
