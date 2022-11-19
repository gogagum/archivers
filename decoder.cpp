#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "include/arithmetic_decoder_decoded.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/bytes_symbol.hpp"

int main(int argc, char* argv[]) {
    /**
    assert(argc == 3 && "Wrong number of arguments.");

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];
    */

    std::string fileIn = "fout.txt";
    std::string fileOut = "small-image-decoded.png";

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

    auto decoded = garchiever::ArithmeticDecoderDecoded(std::move(finData));
    std::uint8_t symByteLen = decoded.takeT<std::uint8_t>();

    std::cerr << "Word bytes length: "
              << static_cast<unsigned int>(symByteLen) << std::endl;

    std::vector<std::byte> res;

    switch (symByteLen) {
    case 1: {
            auto decoder = garchiever::ArithmeticDecoder<garchiever::BytesSymbol<1>>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    case 2: {
            auto decoder = garchiever::ArithmeticDecoder<garchiever::BytesSymbol<2>>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    case 3: {
            auto decoder = garchiever::ArithmeticDecoder<garchiever::BytesSymbol<3>>(std::move(decoded));
            res = decoder.decode();
        }
        break;
    case 4: {
            auto decoder = garchiever::ArithmeticDecoder<garchiever::BytesSymbol<4>>(std::move(decoded));
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
