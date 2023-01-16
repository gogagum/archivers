#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "file_opener.hpp"
#include "include/data_parser.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/word/bytes_word.hpp"
#include "include/word/bits_word.hpp"
#include "include/dictionary/adaptive/adaptive_dictionary.hpp"
#include "include/dictionary/adaptive/adaptive_dictionary_deserialize_constructor.hpp"
#include "include/byte_data_constructor.hpp"

template <std::uint8_t numBytes>
using BytesWord = ga::w::BytesWord<numBytes>;

template <std::uint8_t numBytes>
using BytesDict = ga::dict::AdaptiveDictionary<
    BytesWord<numBytes>,
    typename ga::impl::CountTChoose<BytesWord<numBytes>>::Type
>;

template <std::uint8_t numBytes>
using BytesDictConstructor = ga::dict::construct::AdaptiveDictionaryDeserializeConstructor<
    BytesDict<numBytes>,
    ga::DataParser
>;

template <std::uint8_t numBytes>
using BytesDecoder = ga::ArithmeticDecoder<
    BytesWord<numBytes>,
    BytesDict<numBytes>,
    std::uint64_t
>;

template <std::uint16_t numBits>
using BitsWord = ga::w::BitsWord<numBits>;

template <std::uint16_t numBits>
using BitsDict = ga::dict::AdaptiveDictionary<
    BitsWord<numBits>,
    typename ga::impl::CountTChoose<BitsWord<numBits>>::Type
>;

template <std::uint16_t numBits>
using BitsDictConstructor = ga::dict::construct::AdaptiveDictionaryDeserializeConstructor<
    BitsDict<numBits>,
    ga::DataParser
>;

template <std::uint16_t numBits>
using BitsDecoder = ga::ArithmeticDecoder<BitsWord<numBits>, BitsDict<numBits>, std::uint64_t>;

namespace bpo = boost::program_options;

#define BITS_DECODER_CASE(bits) \
    case (bits): \
        packIntoByteDataConstructor(BitsDecoder<(bits)>(decoded, BitsDictConstructor<(bits)>(decoded))); \
        break;

#define BYTES_DECODER_CASE(bytes) \
    case (bytes * 8): \
        packIntoByteDataConstructor(BytesDecoder<(bytes)>(decoded, BytesDictConstructor<(bytes)>(decoded))); \
        break;

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;

    try {
        appOptionsDescr.add_options()
                ("input-file,i", bpo::value(&inFileName)->required(), "In file name.")
                ("out-filename,o", bpo::value(&outFileName)->default_value(inFileName + "-out"), "Out file name.");

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);
    } catch (const std::logic_error& error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    try {
        auto filesOpener = FileOpener(inFileName, outFileName);

        auto decoded = ga::DataParser(filesOpener.getInData());
        auto symBitLen = decoded.takeT<std::uint16_t>();

        std::cout << "Word bits length: "
                  << static_cast<unsigned int>(symBitLen) << std::endl;

        //auto ratio = decoded.takeT<std::uint64_t>();
        //std::cout << "Ratio: "
        //          << static_cast<unsigned int>(ratio) << std::endl;

        auto dataConstructor = ga::ByteDataConstructor();

        const auto packIntoByteDataConstructor = [&dataConstructor, &filesOpener](auto&& decoder) {
            auto ret = decoder.decode();
            for (auto& word: ret.syms) {
                word.bitsOut(dataConstructor.getBitBackInserter());
            }
            std::copy(ret.tail.begin(), ret.tail.end(),
                      dataConstructor.getBitBackInserter());
            filesOpener.getOutFileStream().write(dataConstructor.data<char>(), dataConstructor.size());
        };

        switch (symBitLen) {
            BYTES_DECODER_CASE(1);
            //BITS_DECODER_CASE(9);
            //BITS_DECODER_CASE(10);
            //BITS_DECODER_CASE(11);
            //BITS_DECODER_CASE(12);
            //BITS_DECODER_CASE(13);
            //BITS_DECODER_CASE(14);
            //BITS_DECODER_CASE(15);
            BYTES_DECODER_CASE(2);
            //BITS_DECODER_CASE(17);
            //BITS_DECODER_CASE(18);
            //BITS_DECODER_CASE(19);
            //BITS_DECODER_CASE(20);
            //BITS_DECODER_CASE(21);
            //BITS_DECODER_CASE(22);
            //BITS_DECODER_CASE(23);
            BYTES_DECODER_CASE(3);
            //BITS_DECODER_CASE(25);
            //BITS_DECODER_CASE(26);
            //BITS_DECODER_CASE(27);
            //BITS_DECODER_CASE(28);
            //BITS_DECODER_CASE(29);
            //BITS_DECODER_CASE(30);
            //BITS_DECODER_CASE(31);
        default:
            throw std::runtime_error((boost::format("bit length %1% is not supported") % symBitLen).str());
            break;
        }
    } catch (const std::runtime_error&  error) {
        std::cout << error.what();
        return 2;
    }

    return 0;
}
