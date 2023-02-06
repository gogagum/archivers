#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "../common.hpp"
#include "arithmetic_archiever_include.hpp"

namespace bpo = boost::program_options;

#define BITS_DECODER_CASE(bits) \
    case (bits): packIntoByteDataConstructor(Dict<bits>(ratio), WordVec<bits>()); break;



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

        const auto ratio = decoded.takeT<std::uint64_t>();
        outStream << "Ratio: " << ratio << std::endl;

        const auto wordsCount = decoded.takeT<std::uint64_t>();
        outStream << "Words count: " << wordsCount << std::endl;

        const auto bitsCount = decoded.takeT<std::uint64_t>();
        outStream << "Bits count: " << bitsCount << std::endl;

        auto dataConstructor = ga::ByteDataConstructor();
        auto decoder = ga::ArithmeticDecoder();

        const auto packIntoByteDataConstructor = [&]<class DictT>(
                    DictT&& dict, auto&& words) {
            decoder.decode(decoded, dict, std::back_inserter(words),
                           wordsCount, bitsCount, outStream);
            if constexpr (std::is_same_v<typename DictT::Word::OutType,
                                         ga::w::tags::BitsOut>) {
                for (const auto& word: words) { \
                    word.bitsOut(dataConstructor.getBitBackInserter());
                }
            } else if constexpr (std::is_same_v<typename DictT::Word::OutType,
                                                ga::w::tags::BytesOut>) {
                for (const auto& word: words) { \
                    word.bytesOut(dataConstructor.getByteBackInserter());
                }
            }
        };

        switch (symBitLen) {
            BITS_DECODER_CASE(8);
            BITS_DECODER_CASE(9);
            BITS_DECODER_CASE(10);
            BITS_DECODER_CASE(11);
            BITS_DECODER_CASE(12);
            BITS_DECODER_CASE(13);
            BITS_DECODER_CASE(14);
            BITS_DECODER_CASE(15);
            BITS_DECODER_CASE(16);
            BITS_DECODER_CASE(17);
            BITS_DECODER_CASE(18);
            BITS_DECODER_CASE(19);
            BITS_DECODER_CASE(20);
            BITS_DECODER_CASE(21);
            BITS_DECODER_CASE(22);
            BITS_DECODER_CASE(23);
            BITS_DECODER_CASE(24);
            BITS_DECODER_CASE(25);
            BITS_DECODER_CASE(26);
            BITS_DECODER_CASE(27);
            BITS_DECODER_CASE(28);
            BITS_DECODER_CASE(29);
            BITS_DECODER_CASE(30);
            BITS_DECODER_CASE(31);
            BITS_DECODER_CASE(32);
        default:
            throw UnsupportedDecodeBitsMode(symBitLen);
            break;
        }

        std::copy(decoded.getCurrPosBitsIter(), decoded.getCurrPosBitsIter() + tailSize,
                  dataConstructor.getBitBackInserter());

        filesOpener.getOutFileStream().write(
                    dataConstructor.data<char>(), dataConstructor.size());
    } catch (const std::runtime_error&  error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
