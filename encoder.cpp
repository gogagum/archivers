#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "include/arithmetic_coder.hpp"
#include "include/byte_flow.hpp"
#include "include/dictionary/static_dictionary.hpp"
#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

int main(int argc, char* argv[]) {
    /**
    assert(argc == 3 && "Wrong number of arguments.");

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];
    */

    std::string fileIn = "slava-durke.png";
    std::string fileOut = "fout.txt";

    std::ifstream fin{fileIn, std::ifstream::binary};

    // Stop eating new lines in binary mode!!!
    fin.unsetf(std::ios::skipws);

    // get its size:
    std::streampos finSize;

    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    std::cerr << "File size: " << finSize << std::endl;
    fin.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<std::byte> finData;
    finData.resize(finSize);

    // Read data
    fin.read(reinterpret_cast<char*>(finData.data()), finData.size());

    auto byteFlow = ga::ByteFlow<ga::BytesSymbol<2>>(
            reinterpret_cast<const std::byte*>(finData.data()), finData.size());
    auto coder = ga::ArithmeticCoder<
                     ga::ByteFlow<ga::BytesSymbol<2>>,
                     ga::dict::AdaptiveDictionary<ga::BytesSymbol<2>>,
                     std::uint64_t>(std::move(byteFlow));
    auto encoded = coder.encode();

    std::ofstream fout;
    fout.open(fileOut, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<const char*>(encoded.data()), encoded.bytesSize());
    fout.close();

    return 0;
}
