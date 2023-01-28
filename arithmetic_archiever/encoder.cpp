#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include "../common.hpp"
#include "encoder_impl.hpp"

#define BITS_CASE(bits, fileOpener, ratio, outStream) \
    case (bits): AdaptiveEncodeImpl<bits>::process((fileOpener), (ratio), (outStream)); break;

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

        switch (numBits) {
            BITS_CASE(8, fileOpener, ratio, outStream);
            BITS_CASE(9, fileOpener, ratio, outStream);
            BITS_CASE(10, fileOpener, ratio, outStream);
            BITS_CASE(11, fileOpener, ratio, outStream);
            BITS_CASE(12, fileOpener, ratio, outStream);
            BITS_CASE(13, fileOpener, ratio, outStream);
            BITS_CASE(14, fileOpener, ratio, outStream);
            BITS_CASE(15, fileOpener, ratio, outStream);
            BITS_CASE(16, fileOpener, ratio, outStream);
            BITS_CASE(17, fileOpener, ratio, outStream);
            BITS_CASE(18, fileOpener, ratio, outStream);
            BITS_CASE(19, fileOpener, ratio, outStream);
            BITS_CASE(20, fileOpener, ratio, outStream);
            BITS_CASE(21, fileOpener, ratio, outStream);
            BITS_CASE(22, fileOpener, ratio, outStream);
            BITS_CASE(23, fileOpener, ratio, outStream);
            BITS_CASE(24, fileOpener, ratio, outStream);
            BITS_CASE(25, fileOpener, ratio, outStream);
            BITS_CASE(26, fileOpener, ratio, outStream);
            BITS_CASE(27, fileOpener, ratio, outStream);
            BITS_CASE(28, fileOpener, ratio, outStream);
            BITS_CASE(29, fileOpener, ratio, outStream);
            BITS_CASE(30, fileOpener, ratio, outStream);
            BITS_CASE(31, fileOpener, ratio, outStream);
            BITS_CASE(32, fileOpener, ratio, outStream);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 2;
    }

    return 0;
}
