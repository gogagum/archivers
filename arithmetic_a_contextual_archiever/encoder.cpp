#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include <ael/arithmetic_coder.hpp>
#include <ael/dictionary/adaptive_a_contextual_dictionary.hpp>

#include <applib/opt_ostream.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <applib/file_opener.hpp>

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;
    std::uint16_t ctxCellsCnt;
    std::uint16_t ctxCellLength;
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
                "cells-cnt,c",
                bpo::value(&ctxCellsCnt)->default_value(4),
                "Contect cells count."
            ) (
                "cell-length,q",
                bpo::value(&ctxCellLength)->default_value(8),
                "Context length."
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
        auto dict = ael::dict::AdaptiveAContextualDictionary(numBits, ctxCellsCnt, ctxCellLength);

        auto [wordsOrds, tail] = OrdAndTailSplitter::process(fileOpener.getInData(), numBits);

        auto coder = ael::ArithmeticCoder();
        auto encoded = ael::ByteDataConstructor();
        encoded.putT<std::uint16_t>(numBits);
        encoded.putT<std::uint16_t>(tail.size());
        encoded.putT<std::uint8_t>(ctxCellsCnt);
        encoded.putT<std::uint8_t>(ctxCellLength);
        const auto wordsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        const auto bitsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        auto [wordsCount, bitsCount] = coder.encode(wordsOrds, encoded, dict, outStream);
        encoded.putTToPosition(wordsCount, wordsCountPos);
        encoded.putTToPosition(bitsCount, bitsCountPos);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}
