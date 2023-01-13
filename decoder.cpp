#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

#include "file_opener.hpp"
#include "include/data_parser.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/word/bytes_word.hpp"
#include "include/word/bits_word.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"
#include "include/byte_data_constructor.hpp"

template <std::uint8_t numBytes>
using BytesWord = ga::w::BytesWord<numBytes>;

template <std::uint8_t numBytes>
using BytesDict = ga::dict::AdaptiveDictionary<BytesWord<numBytes>, typename ga::impl::CountTChoose<BytesWord<numBytes>>::Type, 4>;

template <std::uint8_t numBytes>
using BytesDecoder = ga::ArithmeticDecoder<BytesWord<numBytes>, BytesDict<numBytes>, std::uint64_t>;

template <std::uint16_t numBits>
using BitsWord = ga::w::BitsWord<numBits>;

template <std::uint16_t numBits>
using BitsDict = ga::dict::AdaptiveDictionary<BitsWord<numBits>, typename ga::impl::CountTChoose<BitsWord<numBits>>::Type, 4>;

template <std::uint16_t numBits>
using BitsDecoder = ga::ArithmeticDecoder<BitsWord<numBits>, BitsDict<numBits>, std::uint64_t>;

#define BITS_DECODER_CASE(bits) \
    case (bits): \
        packIntoByteDataConstructor(BitsDecoder<(bits)>(std::move(decoded))); \
        break;

#define BYTES_DECODER_CASE(bytes) \
    case (bytes * 8): \
        packIntoByteDataConstructor(BytesDecoder<(bytes)>(std::move(decoded))); \
        break;

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    assert(argc == 3 && "Wrong number of arguments.");

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <decoded> <result>" << std::endl;
        return 0;
    }

    auto filesOpener = FileOpener(argv[1], argv[2]);

    auto decoded = ga::DataParser(filesOpener.getInData());
    std::uint16_t symBitLen = decoded.takeT<std::uint16_t>();

    std::cerr << "Word bytes length: "
              << static_cast<unsigned int>(symBitLen) << std::endl;

    auto dataConstructor = ga::ByteDataConstructor();

    const auto packIntoByteDataConstructor = [&dataConstructor](auto&& decoder) {
        auto ret = decoder.decode();
        for (auto& word: ret.syms) {
            word.bitsOut(dataConstructor.getBitBackInserter());
        }
        std::copy(ret.tail.begin(), ret.tail.end(),
                  dataConstructor.getBitBackInserter());
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
        BITS_DECODER_CASE(33);
        BITS_DECODER_CASE(34);
        BITS_DECODER_CASE(35);
        BITS_DECODER_CASE(36);
        BITS_DECODER_CASE(37);
        BITS_DECODER_CASE(38);
        BITS_DECODER_CASE(39);
        BYTES_DECODER_CASE(5);
    default:
        assert(false);
        break;
    }

    filesOpener.getOutFileStream().write(reinterpret_cast<const char*>(dataConstructor.data()), dataConstructor.bytesSize());

    return 0;
}
