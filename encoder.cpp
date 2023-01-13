#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "file_opener.hpp"
#include "include/arithmetic_coder.hpp"
#include "include/flow/bytes_word_flow.hpp"
//#include "include/dictionary/static_dictionary.hpp"
//#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"

using Sym = ga::w::BytesWord<2>;
using Dict = ga::dict::AdaptiveDictionary<Sym, ga::impl::CountTChoose<Sym>::Type, 1>;
using Coder = ga::ArithmeticCoder<ga::fl::BytesWordFlow<Sym>, Dict, std::uint64_t>;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <file-to-compress> <result>" << std::endl;
        return 0;
    }

    auto fileOpener = FileOpener(argv[1], argv[2]);

    auto byteFlow = ga::fl::BytesWordFlow<Sym>(fileOpener.getInData());
    auto coder = Coder(std::move(byteFlow));
    auto encoded = coder.encode();

    fileOpener.getOutFileStream().write(reinterpret_cast<const char*>(encoded.data()), encoded.bytesSize());

    return 0;
}
