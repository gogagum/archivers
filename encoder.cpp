#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "file_opener.hpp"
#include "arithmetic_archiever_include.hpp"

#define BYTES_CASE(bytes) \
    case (bytes) * 8: \
        encodeImpl(BytesCoder<(bytes)>( \
            BytesFlow<(bytes)>(fileOpener.getInData()), \
            [ratio]() { return BytesDict<(bytes)>(ratio); }) \
        ); \
        break;

#define BITS_CASE(bits) \
    case (bits): \
        encodeImpl(BitsCoder<(bits)>( \
            BitsFlow<(bits)>(fileOpener.getInData()), \
            [ratio]() { return BitsDict<(bits)>(ratio); }) \
        ); \
    break;

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {

    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;
    std::uint64_t ratio;

    try {
        appOptionsDescr.add_options()
            ("input-file,i", bpo::value(&inFileName)->required(), "In file name.")
            ("out-filename,o", bpo::value(&outFileName)->default_value(inFileName + "-out"), "Out file name.")
            ("bits,b", bpo::value(&numBits)->default_value(16), "Word bits count.")
            ("ratio,r", bpo::value(&ratio)->default_value(2), "Dictionary ratio.");

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);
    } catch (const std::logic_error& error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    try {
        auto fileOpener = FileOpener(inFileName, outFileName);

        const auto encodeImpl = [&fileOpener](auto&& coder) {
            auto encoded = coder.encode();
            fileOpener.getOutFileStream().write(encoded.template data<char>(), encoded.size());
        };

        switch (numBits) {
            BYTES_CASE(1);
            BITS_CASE(9);
            BITS_CASE(10);
            BITS_CASE(11);
            BITS_CASE(12);
            BITS_CASE(13);
            BITS_CASE(14);
            BITS_CASE(15);
            BYTES_CASE(2);
            BITS_CASE(17);
            BITS_CASE(18);
            BITS_CASE(19);
            BITS_CASE(20);
            BITS_CASE(21);
            BITS_CASE(22);
            BITS_CASE(23);
            BYTES_CASE(3);
            BITS_CASE(25);
            BITS_CASE(26);
            BITS_CASE(27);
            BITS_CASE(28);
            BITS_CASE(29);
            BITS_CASE(30);
            BITS_CASE(31);
        default:
            assert(false);
            break;
        }
    } catch (const std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return 2;
    }

    return 0;
}
