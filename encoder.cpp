#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "include/arithmetic_coder.hpp"
#include "include/byte_flow.hpp"

int main(int argc, char* argv[]) {
    assert(argc == 3 && "Wrong number of arguments.");

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];

    std::ifstream fin;
    fin.open(fileIn, std::ios::binary | std::ios::in);

    auto inFileData
            = std::vector<char>(std::istreambuf_iterator<char>(fin),
                                std::istreambuf_iterator<char>());

    auto byteFlow = garchiever::ByteFlow<garchiever::BytesSymbol<1>>(reinterpret_cast<const std::byte*>(inFileData.data()), inFileData.size());
    auto coder = garchiever::ArithmeticCoder(std::move(byteFlow));
    auto encoded = coder.encode();

    std::ofstream fout;
    fout.open(fileOut, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<const char*>(encoded.data()), encoded.bytesSize());
    fout.close();

    return 0;
}
