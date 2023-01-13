#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "include/arithmetic_coder.hpp"
#include "include/flow/bytes_word_flow.hpp"
//#include "include/dictionary/static_dictionary.hpp"
//#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

using Sym = ga::w::BytesWord<3>;
using Dict = ga::dict::AdaptiveDictionary<Sym, ga::impl::CountTChoose<Sym>::Type, 2>;
using Coder = ga::ArithmeticCoder<ga::fl::BytesWordFlow<Sym>, Dict, std::uint64_t>;

class FileOpener {
public:
    FileOpener(std::string fileInName, std::string fileOutName) {
        std::ifstream fin(fileInName, std::ifstream::binary);
        if (!fin.is_open()) {
            std::cout << "Could not open file " << fileInName << "." << std::endl;
            //return 0;
        }

        // Stop eating new lines in binary mode!!!
        fin.unsetf(std::ios::skipws);
        std::streampos finSize;

        // get its size:
        fin.seekg(0, std::ios::end);
        finSize = fin.tellg();
        std::cerr << "File size: " << finSize << std::endl;
        fin.seekg(0, std::ios::beg);

        _inData.resize(finSize);

        // Read data
        fin.read(reinterpret_cast<char*>(_inData.data()), _inData.size());

        _fout.open(fileOutName, std::ios::binary | std::ios::out);
        if (!_fout.is_open()) {
            std::cout << "Could not open file " << fileOutName << "." << std::endl;
            //return 0;
        }
    }

    std::span<const std::byte> getData() {
        return _inData;
    }

    std::ofstream& getFileOut() {
        return _fout;
    }

private:
    std::vector<std::byte> _inData;
    std::ofstream _fout;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <file-to-compress> <result>" << std::endl;
        return 0;
    }

    auto fileOpener = FileOpener(argv[1], argv[2]);

    auto byteFlow = ga::fl::BytesWordFlow<Sym>(fileOpener.getData());
    auto coder = Coder(std::move(byteFlow));
    auto encoded = coder.encode();


    fileOpener.getFileOut().write(reinterpret_cast<const char*>(encoded.data()), encoded.bytesSize());

    return 0;
}
