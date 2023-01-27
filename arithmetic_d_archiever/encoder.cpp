#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include "../common.hpp"
#include "encoder_impl.hpp"

#define BYTES_CASE(bytes, fileOpener) \
    case (bytes) * 8: FileBytesDAdaptiveEncodeImpl<bytes>::process(fileOpener); break;

#define BITS_CASE(bits, fileOpener) \
    case (bits): FileBitsDAdaptiveEncodeImpl<bits>::process(fileOpener); break;

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;

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
            );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        if (outFileName == "") {
            outFileName = inFileName + "-encoded";
        }

        auto fileOpener = FileOpener(inFileName, outFileName);

        switch (numBits) {
            BYTES_CASE(1, fileOpener);
            BITS_CASE(9, fileOpener);
            BITS_CASE(10, fileOpener);
            BITS_CASE(11, fileOpener);
            BITS_CASE(12, fileOpener);
            BITS_CASE(13, fileOpener);
            BITS_CASE(14, fileOpener);
            BITS_CASE(15, fileOpener);
            BYTES_CASE(2, fileOpener);
            BITS_CASE(17, fileOpener);
            BITS_CASE(18, fileOpener);
            BITS_CASE(19, fileOpener);
            BITS_CASE(20, fileOpener);
            BITS_CASE(21, fileOpener);
            BITS_CASE(22, fileOpener);
            BITS_CASE(23, fileOpener);
            BYTES_CASE(3, fileOpener);
            BITS_CASE(25, fileOpener);
            BITS_CASE(26, fileOpener);
            BITS_CASE(27, fileOpener);
            BITS_CASE(28, fileOpener);
            BITS_CASE(29, fileOpener);
            BITS_CASE(30, fileOpener);
            BITS_CASE(31, fileOpener);
            BYTES_CASE(4, fileOpener);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }
    } catch (const std::exception& error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    return 0;
}
