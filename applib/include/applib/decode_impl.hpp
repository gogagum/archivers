#ifndef APPLIB_DECODE_IMPL_HPP
#define APPLIB_DECODE_IMPL_HPP

#include <boost/timer/progress_display.hpp>
#include <cstdint>
#include <ostream>
#include <vector>

#include <ael/arithmetic_decoder.hpp>
#include <ael/data_parser.hpp>

#include "file_opener.hpp"
#include "opt_ostream.hpp"
#include "word_packer.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The DecodeImpl class. Includes decode steps.
///
struct DecodeImpl {
    struct ConfigureRet {
        optout::OptOstreamRef outStream;
        FileOpener fileOpener;
        ael::DataParser decoded;
    };

    static ConfigureRet configure(int argc, char* argv[]);

    static void process(ael::DataParser& decoded,
                        auto& dict,
                        std::size_t wordsCount,
                        std::size_t bitsCount,
                        std::uint16_t symBitLen,
                        std::uint16_t tailSize,
                        std::ostream& bytesOutStream,
                        optout::OptOstreamRef optLogOutStream);
};

////////////////////////////////////////////////////////////////////////////////
void DecodeImpl::process(ael::DataParser &decoded,
                         auto &dict,
                         std::size_t wordsCount,
                         std::size_t bitsCount,
                         std::uint16_t symBitLen,
                         std::uint16_t tailSize,
                         std::ostream &bytesOutStream,
                         optout::OptOstreamRef optLogOutStream) {
    auto dataConstructor = ael::ByteDataConstructor();
    
    std::vector<std::uint64_t> ords;
    
    auto progressBar = boost::timer::progress_display(wordsCount);
    ael::ArithmeticDecoder::decode(
        decoded, dict, std::back_inserter(ords),
        wordsCount, bitsCount, [&progressBar](){ ++progressBar; });
    
    WordPacker::process(ords, dataConstructor, symBitLen);
    
    std::copy(decoded.getEndBitsIter() - tailSize, decoded.getEndBitsIter(),
              dataConstructor.getBitBackInserter());
    
    bytesOutStream.write(dataConstructor.data<char>(), dataConstructor.size());
}

#endif  // APPLIB_DECODE_IMPL_HPP
