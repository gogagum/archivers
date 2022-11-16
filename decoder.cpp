#include <cassert>
#include <string>
#include <fstream>
#include <vector>

#include "include/arithmetic_decoder_decoded.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/bytes_symbol.hpp"

int main(int argc, char* argv[]) {
    assert(argc == 3 && "Wrong number of arguments.");

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];

    std::basic_ifstream<std::byte> fin;
    fin.open(fileIn, std::ios::binary | std::ios::in);

    auto inFileData
            = std::vector<std::byte>(std::istreambuf_iterator<std::byte>(fin),
                                     std::istreambuf_iterator<std::byte>());

    auto decoded = garchiever::ArithmeticDecoderDecoded(std::move(inFileData));
    std::uint8_t symByteLen = decoded.takeT<std::uint8_t>();

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
    }

    std::ofstream fout;
    fout.open(fileOut, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<const char*>(res.data()), res.size());
    fout.close();

    return 0;
}
