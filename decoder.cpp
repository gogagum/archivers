#include <cassert>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

#include "include/data_parser.hpp"
#include "include/arithmetic_decoder.hpp"
#include "include/word/bytes_word.hpp"
//#include "include/dictionary/static_dictionary.hpp"
//#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"
#include "include/byte_data_constructor.hpp"

template <std::uint8_t numBytes>
using Sym = ga::w::BytesWord<numBytes>;

template <std::uint8_t numBytes>
using Dict = ga::dict::AdaptiveDictionary<Sym<numBytes>, typename ga::impl::CountTChoose<Sym<numBytes>>::Type>;

template <std::uint8_t numBytes>
using Decoder = ga::ArithmeticDecoder<Sym<numBytes>, Dict<numBytes>, std::uint64_t>;

class FilesOpener{
public:
    FilesOpener(std::string inFileName, std::string outFileName) {
        _openInFile(inFileName);
        _openOutFile(outFileName);
    }

    std::span<std::byte> getInData() {
        return std::span<std::byte>(_finData.begin(), _finData.end());
    }

    std::ofstream& getOutstream() {
        return _fout;
    }

private:
    void _openInFile(const std::string& fileInName) {
        std::ifstream fin{fileInName, std::ifstream::ate | std::ifstream::binary};
        if (!fin.is_open()) {
            std::cout << "Could not open file: " << fileInName << std::endl;
            // TODO throw
        }
        fin.unsetf(std::ios::skipws);
        // get its size:
        std::streampos finSize;

        fin.seekg(0, std::ios::end);
        finSize = fin.tellg();
        std::cerr << "File size: " << finSize << "." << std::endl;
        fin.seekg(0, std::ios::beg);

        _finData.resize(finSize);

        fin.read(reinterpret_cast<char*>(_finData.data()), finSize);
    }

    void _openOutFile(std::string& fileOutName) {
        _fout.open(fileOutName, std::ios::binary | std::ios::out);
        if (!_fout.is_open()) {
            std::cout << "Could not open file: " << fileOutName << "." << std::endl;
            // TODO throw
        }
    }

private:
    std::vector<std::byte> _finData;
    std::ofstream _fout;
};



//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    assert(argc == 3 && "Wrong number of arguments.");

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <decoded> <result>" << std::endl;
        return 0;
    }

    auto filesOpener = FilesOpener(argv[1], argv[2]);

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
    case 8:
        packIntoByteDataConstructor(Decoder<1>(std::move(decoded)));
        break;
    case 16:
        packIntoByteDataConstructor(Decoder<2>(std::move(decoded)));
        break;
    case 24:
        packIntoByteDataConstructor(Decoder<3>(std::move(decoded)));
        break;
    case 32:
        packIntoByteDataConstructor(Decoder<4>(std::move(decoded)));
        break;
    default:
        assert(false);
        break;
    }

    filesOpener.getOutstream().write(reinterpret_cast<const char*>(dataConstructor.data()), dataConstructor.bytesSize());

    return 0;
}
