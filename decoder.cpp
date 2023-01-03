#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "include/arithmetic_decoder_decoded.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/word/bytes_symbol.hpp"
//#include "include/dictionary/static_dictionary.hpp"
//#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

template <std::uint8_t numBytes>
using Sym = ga::w::BytesSymbol<numBytes>;

template <std::uint8_t numBytes>
using Dict = ga::dict::AdaptiveDictionary<Sym<numBytes>>;

template <std::uint8_t numBytes>
using Decoder = ga::ArithmeticDecoder<Sym<numBytes>, Dict<numBytes>, std::uint64_t>;

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    assert(argc == 3 && "Wrong number of arguments.");

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <decoded> <result>" << std::endl;
        return 0;
    }

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];

    std::ifstream fin{fileIn, std::ifstream::ate | std::ifstream::binary};
    if (!fin.is_open()) {
        std::cout << "Could not open file: " << fileIn << std::endl;
        return 0;
    }

    fin.unsetf(std::ios::skipws);

    // get its size:
    std::streampos finSize;

    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    std::cerr << "File size: " << finSize << "." << std::endl;
    fin.seekg(0, std::ios::beg);

    std::vector<std::byte> finData;
    finData.resize(finSize);

    fin.read(reinterpret_cast<char*>(finData.data()), finSize);

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(finData));
    std::uint16_t symBitLen = decoded.takeT<std::uint16_t>();

    std::cerr << "Word bytes length: "
              << static_cast<unsigned int>(symBitLen) << std::endl;

    std::vector<std::byte> res;

    switch (symBitLen) {
    case 8: {
            auto decoder = Decoder<1>(std::move(decoded));
            auto ret = decoder.decode();
            auto symsSize = ret.syms.size();
            res.resize(symsSize + ret.tail.size());
            std::memcpy(res.data(), ret.syms.data(), symsSize);
            std::memcpy(res.data() + symsSize, ret.tail.data(), ret.tail.size());
        }
        break;
    case 16: {
            auto decoder = Decoder<2>(std::move(decoded));
            auto ret = decoder.decode();
            auto symsSize = ret.syms.size() * 2;
            res.resize(symsSize + ret.tail.size());
            std::memcpy(res.data(), ret.syms.data(), symsSize);
            std::memcpy(res.data() + symsSize, ret.tail.data(), ret.tail.size());
        }
        break;
    case 24: {
            auto decoder = Decoder<3>(std::move(decoded));
            auto ret = decoder.decode();
            auto symsSize = ret.syms.size() * 3;
            res.resize(symsSize + ret.tail.size());
            std::memcpy(res.data(), ret.syms.data(), symsSize);
            std::memcpy(res.data() + symsSize, ret.tail.data(), ret.tail.size());
        }
        break;
    case 32: {
            auto decoder = Decoder<4>(std::move(decoded));
            auto ret = decoder.decode();
            auto symsSize = ret.syms.size() * 3;
            res.resize(symsSize + ret.tail.size());
            std::memcpy(res.data(), ret.syms.data(), symsSize);
            std::memcpy(res.data() + symsSize, ret.tail.data(), ret.tail.size());
        }
        break;
    default:
        assert(false);
        break;
    }

    std::ofstream fout;
    fout.open(fileOut, std::ios::binary | std::ios::out);
    if (!fout.is_open()) {
        std::cout << "Could not open file: " << fileOut << "." << std::endl;
        return 0;
    }
    fout.write(reinterpret_cast<const char*>(res.data()), res.size());
    fout.close();

    return 0;
}
