#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include "../common.hpp"
#include "encoder_impl.hpp"

#define BYTES_CASE(bytes, fileOpener, outStream) \
    case (bytes) * 8: FileBytesDAdaptiveEncodeImpl<bytes>::process(fileOpener, outStream); break;

#define BITS_CASE(bits, fileOpener, outStream) \
    case (bits): FileBitsDAdaptiveEncodeImpl<bits>::process(fileOpener, outStream); break;

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
                bpo::value(&outFileName)->default_value(""),
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

        if (outFileName == "") {
            outFileName = inFileName + "-encoded";
        }

        optout::OptOstreamRef outStream;

        if (logStreamParam == "stdout") {
            outStream = std::cout;
        } else if (logStreamParam == "stderr") {
            outStream = std::cerr;
        } else if (logStreamParam == "off") {
        } else {
            throw InvalidStreamParam(logStreamParam);
        }

        auto fileOpener = FileOpener(inFileName, outFileName, outStream);

        switch (numBits) {
            BYTES_CASE(1, fileOpener, outStream);
            BITS_CASE(9, fileOpener, outStream);
            BITS_CASE(10, fileOpener, outStream);
            BITS_CASE(11, fileOpener, outStream);
            BITS_CASE(12, fileOpener, outStream);
            BITS_CASE(13, fileOpener, outStream);
            BITS_CASE(14, fileOpener, outStream);
            BITS_CASE(15, fileOpener, outStream);
            BYTES_CASE(2, fileOpener, outStream);
            BITS_CASE(17, fileOpener, outStream);
            BITS_CASE(18, fileOpener, outStream);
            BITS_CASE(19, fileOpener, outStream);
            BITS_CASE(20, fileOpener, outStream);
            BITS_CASE(21, fileOpener, outStream);
            BITS_CASE(22, fileOpener, outStream);
            BITS_CASE(23, fileOpener, outStream);
            BYTES_CASE(3, fileOpener, outStream);
            BITS_CASE(25, fileOpener, outStream);
            BITS_CASE(26, fileOpener, outStream);
            BITS_CASE(27, fileOpener, outStream);
            BITS_CASE(28, fileOpener, outStream);
            BITS_CASE(29, fileOpener, outStream);
            BITS_CASE(30, fileOpener, outStream);
            BITS_CASE(31, fileOpener, outStream);
            BYTES_CASE(4, fileOpener, outStream);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}
