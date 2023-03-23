#ifndef APPLIB_DECODE_IMPL_HPP
#define APPLIB_DECODE_IMPL_HPP

#include <cstdint>
#include <ostream>
#include <vector>

#include <indicators/progress_bar.hpp>

#include <ael/arithmetic_decoder.hpp>
#include <ael/data_parser.hpp>

#include "file_opener.hpp"
#include "indicators/setting.hpp"
#include "word_packer.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecodeImpl class. Includes decode steps.
///
struct DecodeImpl {
    struct ConfigureRet {
        std::ostream& outStream;
        FileOpener fileOpener;
        ael::DataParser decoded;
    };

    static std::ostream nullOut;

    static ConfigureRet configure(int argc, char* argv[]);

    static void process(ael::DataParser& decoded,
                        auto& dict,
                        std::size_t wordsCount,
                        std::size_t bitsCount,
                        std::uint16_t symBitLen,
                        std::uint16_t tailSize,
                        std::ostream& bytesOutStream,
                        std::ostream& optLogOutStream);
};

////////////////////////////////////////////////////////////////////////////////
void DecodeImpl::process(ael::DataParser &decoded,
                         auto &dict,
                         std::size_t wordsCount,
                         std::size_t bitsCount,
                         std::uint16_t symBitLen,
                         std::uint16_t tailSize,
                         std::ostream& bytesOutStream,
                         std::ostream& optLogOutStream) {
    auto dataConstructor = ael::ByteDataConstructor();
    
    std::vector<std::uint64_t> ords;
    
    auto progressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{wordsCount},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Decoding"},
        indicators::option::Stream{optLogOutStream});
    ael::ArithmeticDecoder::decode(
        decoded, dict, std::back_inserter(ords),
        wordsCount, bitsCount, [&progressBar](){ progressBar.tick(); });
    
    WordPacker::process(ords, dataConstructor, symBitLen);
    
    std::copy(decoded.getEndBitsIter() - tailSize, decoded.getEndBitsIter(),
              dataConstructor.getBitBackInserter());
    
    bytesOutStream.write(dataConstructor.data<char>(), dataConstructor.size());
}

#endif  // APPLIB_DECODE_IMPL_HPP
