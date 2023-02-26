#include <iostream>
#include <optional>
#include <string>
#include <cstdint>

#include <ael/arithmetic_coder.hpp>
#include <ael/dictionary/adaptive_d_contextual_dictionary_improved.hpp>

#include <applib/opt_ostream.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <applib/file_opener.hpp>

int main(int argc, char* argv[]) {
    std::string inFileName;
    std::string outFileName;
    const std::uint16_t numBits = 8;
    const std::uint16_t ctxCellsCnt = 8;
    const std::uint16_t ctxCellLength = 4;

    try {
        if (argc < 2 || argc > 3) {
            std::stringstream message;            
            message << "Usage: " << std::string(argv[0])
                    << " <input_file> [<output_file>]" << std::endl;
            throw std::invalid_argument(message.str());   
        }
        std::string inFileName = argv[1];
        std::string outFileName = (argc == 3) ? argv[2] : inFileName + "-encoded";

        optout::OptOstreamRef outStream = std::nullopt;
        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
        auto dict = ael::dict::AdaptiveDContextualDictionaryImproved(numBits, ctxCellsCnt, ctxCellLength);

        auto [wordsOrds, tail] = OrdAndTailSplitter::process(fileOpener.getInData(), numBits);

        auto coder = ael::ArithmeticCoder();
        auto encoded = ael::ByteDataConstructor();
        encoded.putT<std::uint16_t>(numBits);
        encoded.putT<std::uint16_t>(tail.size());
        encoded.putT<std::uint8_t>(ctxCellsCnt);
        encoded.putT<std::uint8_t>(ctxCellLength);
        const auto wordsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        const auto bitsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        auto [wordsCount, bitsCount] = coder.encode(wordsOrds, encoded, dict, outStream);
        encoded.putTToPosition(wordsCount, wordsCountPos);
        encoded.putTToPosition(bitsCount, bitsCountPos);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}
