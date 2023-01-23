#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/range.hpp>

#include "../common.hpp"
#include "arithmetic_a_archiever_include.hpp"

namespace bpo = boost::program_options;

#define BITS_DECODER_CASE(bits) \
    case (bits): \
        packIntoByteDataConstructor(BitsDecoder<(bits)>(BitsDict<(bits)>())); \
        break;

#define BYTES_DECODER_CASE(bytes) \
    case (bytes * 8): \
        packIntoByteDataConstructor(BytesDecoder<(bytes)>(BytesDict<(bytes)>())); \
        break;

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;

    try {
        appOptionsDescr.add_options() (
            "input-file,i",
            bpo::value(&inFileName)->required(),
            "In file name."
        ) (
            "out-filename,o",
            bpo::value(&outFileName)->default_value(inFileName + "-out"),
            "Out file name."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        auto filesOpener = FileOpener(inFileName, outFileName);
        auto decoded = ga::DataParser(filesOpener.getInData());

        const auto symBitLen = decoded.takeT<std::uint16_t>();
        std::cerr << "Word bits length: " << symBitLen << std::endl;

        const auto tailSize = decoded.takeT<std::uint16_t>();
        std::cerr << "Tail size: " << tailSize << std::endl;

        const auto wordsCount = decoded.takeT<std::uint64_t>();
        std::cerr << "Words count: " << wordsCount << std::endl;

        const auto bitsCount = decoded.takeT<std::uint64_t>();
        std::cerr << "Bits count: " << bitsCount << std::endl;

        auto dataConstructor = ga::ByteDataConstructor();

        const auto packIntoByteDataConstructor =
                [&dataConstructor, &decoded, wordsCount, bitsCount] (auto&& decoder) {
            for (auto& word: decoder.decode(decoded, wordsCount, bitsCount)) {
                word.bitsOut(dataConstructor.getBitBackInserter());
            }
        };

        switch (symBitLen) {
            BYTES_DECODER_CASE(1);
            BITS_DECODER_CASE(9);
            BITS_DECODER_CASE(10);
            BITS_DECODER_CASE(11);
            BITS_DECODER_CASE(12);
            BITS_DECODER_CASE(13);
            BITS_DECODER_CASE(14);
            BITS_DECODER_CASE(15);
            BYTES_DECODER_CASE(2);
            BITS_DECODER_CASE(17);
            BITS_DECODER_CASE(18);
            BITS_DECODER_CASE(19);
            BITS_DECODER_CASE(20);
            BITS_DECODER_CASE(21);
            BITS_DECODER_CASE(22);
            BITS_DECODER_CASE(23);
            BYTES_DECODER_CASE(3);
            BITS_DECODER_CASE(25);
            BITS_DECODER_CASE(26);
            BITS_DECODER_CASE(27);
            BITS_DECODER_CASE(28);
            BITS_DECODER_CASE(29);
            BITS_DECODER_CASE(30);
            BITS_DECODER_CASE(31);
            BYTES_DECODER_CASE(4);
        default:
            throw UnsupportedDecodeBitsMode(symBitLen);
            break;
        }

        std::copy(decoded.getCurrPosBitsIter(),
                  decoded.getCurrPosBitsIter() + tailSize,
                  dataConstructor.getBitBackInserter());

        filesOpener.getOutFileStream().write(
                    dataConstructor.data<char>(), dataConstructor.size());
    } catch (const std::runtime_error&  error) {
        std::cout << error.what();
        return 1;
    }

    return 0;
}
