#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <iostream>

#include <ael/dictionary/adaptive_d_contextual_dictionary_improved.hpp>

#include <applib/opt_ostream.hpp>
#include <applib/file_opener.hpp>
#include <applib/decode_impl.hpp>

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    try {
        if (argc < 2 || argc > 3) {
            std::stringstream message;            
            message << "Usage: " << std::string(argv[0])
                    << " <input_file> [<output_file>]" << std::endl;
            throw std::invalid_argument(message.str());   
        }
        std::string inFileName = argv[1];
        std::string outFileName = (argc == 3) ? argv[2] : inFileName + "-decoded";

        optout::OptOstreamRef outStream = std::nullopt;
        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
        auto decoded = ael::DataParser(fileOpener.getInData());

        const auto takeWithLog = [&]<class T>(const std::string& msg, T) {    
            const auto ret = decoded.takeT<T>();
            outStream << msg << static_cast<std::int64_t>(ret) << std::endl;
            return ret;
        };

        const auto symBitLen = takeWithLog("Word bits length: ", std::uint16_t{});
        const auto tailSize = takeWithLog("Tail size: ", std::uint16_t{});
        const auto ctxCellsCnt = takeWithLog("Context cells count: ", std::uint8_t{});
        const auto ctxCellLength = takeWithLog("Context cell bit length: ", std::uint8_t{});
        const auto wordsCount = takeWithLog("Words count: ", std::uint64_t{});
        const auto bitsCount = takeWithLog("Bits count: ", std::uint64_t{});

        auto dict = ael::dict::AdaptiveDContextualDictionaryImproved(symBitLen, ctxCellsCnt, ctxCellLength);

        DecodeImpl::process(decoded, dict, wordsCount, bitsCount, symBitLen, tailSize,
                            fileOpener.getOutFileStream(), outStream);
    } catch (const std::exception&  error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
