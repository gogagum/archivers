#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include "../common.hpp"
#include "encoder_impl.hpp"

#define BITS_CASE(bits, outIter, tail, fileOpener) \
    case (bits): AAdaptiveEncodeImpl<bits>::process((fileOpener), (outIter), (tail)); break;

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;
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
        auto dict = Dict(1ull << numBits);

        std::vector<std::uint64_t> wordsOrds;
        auto outIter = std::back_inserter(wordsOrds);
        auto tail = boost::container::static_vector<bool, 32>();

        switch (numBits) {
            BITS_CASE(8, outIter, tail, fileOpener);
            BITS_CASE(9, outIter, tail, fileOpener);
            BITS_CASE(10, outIter, tail, fileOpener);
            BITS_CASE(11, outIter, tail, fileOpener);
            BITS_CASE(12, outIter, tail, fileOpener);
            BITS_CASE(13, outIter, tail, fileOpener);
            BITS_CASE(14, outIter, tail, fileOpener);
            BITS_CASE(15, outIter, tail, fileOpener);
            BITS_CASE(16, outIter, tail, fileOpener);
            BITS_CASE(17, outIter, tail, fileOpener);
            BITS_CASE(18, outIter, tail, fileOpener);
            BITS_CASE(19, outIter, tail, fileOpener);
            BITS_CASE(20, outIter, tail, fileOpener);
            BITS_CASE(21, outIter, tail, fileOpener);
            BITS_CASE(22, outIter, tail, fileOpener);
            BITS_CASE(23, outIter, tail, fileOpener);
            BITS_CASE(24, outIter, tail, fileOpener);
            BITS_CASE(25, outIter, tail, fileOpener);
            BITS_CASE(26, outIter, tail, fileOpener);
            BITS_CASE(27, outIter, tail, fileOpener);
            BITS_CASE(28, outIter, tail, fileOpener);
            BITS_CASE(29, outIter, tail, fileOpener);
            BITS_CASE(30, outIter, tail, fileOpener);
            BITS_CASE(31, outIter, tail, fileOpener);
            BITS_CASE(32, outIter, tail, fileOpener);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }

        auto coder = ga::ArithmeticCoder();
        auto encoded = ga::ByteDataConstructor();
        encoded.putT<std::uint16_t>(numBits);
        encoded.putT<std::uint16_t>(tail.size());
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
