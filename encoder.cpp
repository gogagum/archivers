#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "include/arithmetic_coder.hpp"
#include "include/flow/bytes_word_flow.hpp"
//#include "include/dictionary/static_dictionary.hpp"
//#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

using Sym = ga::w::BytesSymbol<3>;
using Dict = ga::dict::AdaptiveDictionary<Sym>;
using Coder = ga::ArithmeticCoder<ga::fl::BytesWordFlow<Sym>, Dict, std::uint64_t>;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <file-to-compress> <result>" << std::endl;
        return 0;
    }

    std::string fileIn = argv[1];
    std::string fileOut = argv[2];

    std::ifstream fin{fileIn, std::ifstream::binary};
    if (!fin.is_open()) {
        std::cout << "Could not open file " << fileIn << "." << std::endl;
        return 0;
    }

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

    auto byteFlow = ga::fl::BytesWordFlow<Sym>(finData.data(), finData.size());
    auto coder = Coder(std::move(byteFlow));
    auto encoded = coder.encode();

    std::ofstream fout(fileOut, std::ios::binary | std::ios::out);
    if (!fout.is_open()) {
        std::cout << "Could not open file " << fileOut << "." << std::endl;
        return 0;
    }
    fout.write(reinterpret_cast<const char*>(encoded.data()), encoded.bytesSize());
    fout.close();

    return 0;
}
