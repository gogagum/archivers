#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include "../common.hpp"
#include "encoder_impl.hpp"

#define BYTES_CASE(bytes, fileOpener, ratio) \
    case (bytes) * 8: FileBytesAdaptiveEncodeImpl<bytes>::process((fileOpener), (ratio)); break;

#define BITS_CASE(bits, fileOpener, ratio) \
    case (bits): FileBitsAdaptiveEncodeImpl<bits>::process((fileOpener), (ratio)); break;

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;
    std::uint64_t ratio;

    try {
        appOptionsDescr.add_options() (
                "input-file,i",
                bpo::value(&inFileName)->required(),
                "In file name."
            ) (
                "out-filename,o",
                bpo::value(&outFileName)->default_value(inFileName + "-out"),
                "Out file name."
            ) (
                "bits,b",
                bpo::value(&numBits)->default_value(16),
                "Word bits count."
            ) (
                "ratio,r",
                bpo::value(&ratio)->default_value(2),
                "Dictionary ratio."
            );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        auto fileOpener = FileOpener(inFileName, outFileName);

        switch (numBits) {
            BYTES_CASE(1, fileOpener, ratio);
            BITS_CASE(9, fileOpener, ratio);
            BITS_CASE(10, fileOpener, ratio);
            BITS_CASE(11, fileOpener, ratio);
            BITS_CASE(12, fileOpener, ratio);
            BITS_CASE(13, fileOpener, ratio);
            BITS_CASE(14, fileOpener, ratio);
            BITS_CASE(15, fileOpener, ratio);
            BYTES_CASE(2, fileOpener, ratio);
            BITS_CASE(17, fileOpener, ratio);
            BITS_CASE(18, fileOpener, ratio);
            BITS_CASE(19, fileOpener, ratio);
            BITS_CASE(20, fileOpener, ratio);
            BITS_CASE(21, fileOpener, ratio);
            BITS_CASE(22, fileOpener, ratio);
            BITS_CASE(23, fileOpener, ratio);
            BYTES_CASE(3, fileOpener, ratio);
            BITS_CASE(25, fileOpener, ratio);
            BITS_CASE(26, fileOpener, ratio);
            BITS_CASE(27, fileOpener, ratio);
            BITS_CASE(28, fileOpener, ratio);
            BITS_CASE(29, fileOpener, ratio);
            BITS_CASE(30, fileOpener, ratio);
            BITS_CASE(31, fileOpener, ratio);
            BYTES_CASE(4, fileOpener, ratio);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }
    } catch (const std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return 2;
    }

    return 0;
}
