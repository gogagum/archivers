#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "file_opener.hpp"
#include "include/arithmetic_coder.hpp"
#include "include/word/bytes_word.hpp"
#include "include/flow/bytes_word_flow.hpp"
#include "include/word/bits_word.hpp"
#include "include/flow/bits_word_flow.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

template <std::uint8_t numBytes>
using BytesWord = ga::w::BytesWord<numBytes>;

template <std::uint8_t numBytes>
using BytesFlow = ga::fl::BytesWordFlow<BytesWord<numBytes>>;

template <std::uint8_t numBytes>
using BytesDict = ga::dict::AdaptiveDictionary<BytesWord<numBytes>, typename ga::impl::CountTChoose<BytesWord<numBytes>>::Type, 8>;

template <std::uint8_t numBytes>
using BytesCoder = ga::ArithmeticCoder<BytesFlow<numBytes>, BytesDict<numBytes>, std::uint64_t>;

template <std::uint16_t numBits>
using BitsWord = ga::w::BitsWord<numBits>;

template <std::uint16_t numBits>
using BitsFlow = ga::fl::BitsWordFlow<BitsWord<numBits>>;

template <std::uint16_t numBits>
using BitsDict = ga::dict::AdaptiveDictionary<BitsWord<numBits>, typename ga::impl::CountTChoose<BitsWord<numBits>>::Type, 8>;

template <std::uint16_t numBits>
using BitsCoder = ga::ArithmeticCoder<BitsFlow<numBits>, BitsDict<numBits>, std::uint64_t>;

#define BYTES_CASE(bytes) \
    case (bytes * 8): { \
        auto byteFlow = BytesFlow<(bytes)>(fileOpener.getInData()); \
        auto coder = BytesCoder<(bytes)>(std::move(byteFlow)); \
        auto encoded = coder.encode(); \
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size()); \
    } \
    break;

#define BITS_CASE(bits) \
    case (bits): { \
        auto bitFlow = BitsFlow<(bits)>(fileOpener.getInData()); \
        auto coder = BitsCoder<(bits)>(std::move(bitFlow)); \
        auto encoded = coder.encode(); \
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size()); \
    } \
    break;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <file-to-compress> <result> <bits>" << std::endl;
        return 0;
    }

    auto fileOpener = FileOpener(argv[1], argv[2]);

    std::size_t numBits = (argc == 3) ? 16 : std::stoi(argv[3]);

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
        BYTES_CASE(4);
        BITS_CASE(33);
        BITS_CASE(34);
        BITS_CASE(35);
        BITS_CASE(36);
        BITS_CASE(37);
        BITS_CASE(38);
        BITS_CASE(39);
        BYTES_CASE(5);
    default:
        assert(false);
        break;
    }

    //fileOpener.getOutFileStream().write(reinterpret_cast<const char*>(encoded.data()), encoded.size());

    return 0;
}
