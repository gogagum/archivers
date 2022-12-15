#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "include/arithmetic_decoder_decoded.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/bytes_symbol.hpp"
#include "include/dictionary/static_dictionary.hpp"
#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

template <std::uint8_t numBytes>
using Decoder = ga::ArithmeticDecoder<
    ga::BytesSymbol<numBytes>,
    ga::dict::AdaptiveDictionary<ga::BytesSymbol<numBytes>>>;

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    /**
    assert(argc == 3 && "Wrong number of arguments.");

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];
    */

    std::string fileIn = "fout.txt";
    std::string fileOut = "small2_decoded.png";

    std::ifstream fin{fileIn, std::ifstream::ate | std::ifstream::binary};

    fin.unsetf(std::ios::skipws);

    // get its size:
    std::streampos finSize;

    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    std::cerr << "File size: " << finSize << std::endl;
    fin.seekg(0, std::ios::beg);

    std::vector<std::byte> finData;
    finData.resize(finSize);

    fin.read(reinterpret_cast<char*>(finData.data()), finSize);

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(finData));
    std::uint8_t symByteLen = decoded.takeT<std::uint8_t>();

    std::cerr << "Word bytes length: "
              << static_cast<unsigned int>(symByteLen) << std::endl;

    std::vector<std::byte> res;

    switch (symByteLen) {
    case 1: {
            auto decoder = Decoder<1>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    case 2: {
            auto decoder = Decoder<2>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    case 3: {
            auto decoder = Decoder<3>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    case 4: {
            auto decoder = Decoder<4>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    default:
        assert(false);
        break;
    }

    std::ofstream fout;
    fout.open(fileOut, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<const char*>(res.data()), res.size());
    fout.close();

    return 0;
}
