#include <cassert>
#include <cstdint>
#include <string>
#include <iostream>

#include <ael/dictionary/adaptive_a_contextual_dictionary.hpp>

#include <applib/file_opener.hpp>
#include <applib/decode_impl.hpp>

//----------------------------------------------------------------------------//
int main(int argc, char* argv[]) {
    try {
        auto cfg = DecodeImpl::configure(argc, argv);

        const auto takeWithLog = [&]<class T>(const std::string& msg, T) {    
            const auto ret = cfg.decoded.takeT<T>();
            cfg.outStream << msg << static_cast<std::int64_t>(ret) << std::endl;
            return ret;
        };

        const auto symBitLen = takeWithLog("Word bits length: ", std::uint16_t{});
        const auto tailSize = takeWithLog("Tail size: ", std::uint16_t{});
        const auto ctxCellsCnt = takeWithLog("Context cells count: ", std::uint8_t{});
        const auto ctxCellLength = takeWithLog("Context cell bit length: ", std::uint8_t{});
        const auto wordsCount = takeWithLog("Words count: ", std::uint64_t{});
        const auto bitsCount = takeWithLog("Bits count: ", std::uint64_t{});

        auto dict = ael::dict::AdaptiveAContextualDictionary(symBitLen, ctxCellsCnt, ctxCellLength);

        DecodeImpl::process(cfg.decoded, dict, wordsCount, bitsCount, symBitLen, tailSize,
                            cfg.fileOpener.getOutFileStream(), cfg.outStream);
    } catch (const std::exception&  error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
