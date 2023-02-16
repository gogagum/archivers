#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <ael/arithmetic_decoder.hpp>
#include <ael/dictionary/ppmd_dictionary.hpp>
#include "../common.hpp"

namespace bpo = boost::program_options;

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
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
            "log-stream,l",
            bpo::value(&logStreamParam)->default_value("stdout"),
            "Log stream."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        outFileName = outFileName.empty() ? inFileName + "-decoded" : outFileName;
        optout::OptOstreamRef outStream = get_out_stream(logStreamParam);

        auto filesOpener = FileOpener(inFileName, outFileName, outStream);
        auto decoded = ga::DataParser(filesOpener.getInData());

        const auto symBitLen = decoded.takeT<std::uint16_t>();
        outStream << "Word bits length: " << symBitLen << std::endl;

        const auto tailSize = decoded.takeT<std::uint16_t>();
        outStream << "Tail size: " << tailSize << std::endl;

        const auto ctxCellsCnt = decoded.takeT<std::uint8_t>();
        outStream << "Context cells count: "
                  << static_cast<std::uint32_t>(ctxCellsCnt) << std::endl;

        const auto ctxCellLength = decoded.takeT<std::uint8_t>();
        outStream << "Context cell bit length: "
                  << static_cast<std::uint32_t>(ctxCellLength) << std::endl;

        const auto wordsCount = decoded.takeT<std::uint64_t>();
        outStream << "Words count: " << wordsCount << std::endl;

        const auto bitsCount = decoded.takeT<std::uint64_t>();
        outStream << "Bits count: " << bitsCount << std::endl;

        auto dataConstructor = ga::ByteDataConstructor();
        auto decoder = ga::ArithmeticDecoder();
        auto dict = ael::dict::PPMDDictionary(symBitLen, ctxCellsCnt, ctxCellLength);

        std::vector<std::uint64_t> ords;

        decoder.decode(decoded, dict, std::back_inserter(ords),
                       wordsCount, bitsCount, outStream);

        WordPacker::process(ords, dataConstructor, symBitLen);

        std::copy(decoded.getEndBitsIter() - tailSize, decoded.getEndBitsIter(),
                  dataConstructor.getBitBackInserter());

        filesOpener.getOutFileStream().write(
                    dataConstructor.data<char>(), dataConstructor.size());
    } catch (const std::exception&  error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
