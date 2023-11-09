#ifndef APPLIB_DECODE_IMPL_HPP
#define APPLIB_DECODE_IMPL_HPP

#include <ael/arithmetic_decoder.hpp>
#include <ael/data_parser.hpp>
#include <cstdint>
#include <indicators/progress_bar.hpp>
#include <ostream>
#include <vector>

#include "file_opener.hpp"
#include "progress_bar.hpp"
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

  static ConfigureRet configure(int argc, char* argv[]);

  static void process(ael::DataParser& decoded, auto& dict,
                      std::size_t wordsCount, std::size_t bitsCount,
                      std::uint16_t symBitLen, std::uint16_t tailSize,
                      std::basic_ostream<std::byte>& bytesOutStream,
                      std::ostream& optLogOutStream);

 private:
  static std::ostream nullOut;
};

////////////////////////////////////////////////////////////////////////////////
void DecodeImpl::process(ael::DataParser& decoded, auto& dict,
                         std::size_t wordsCount, std::size_t bitsCount,
                         std::uint16_t symBitLen, std::uint16_t tailSize,
                         std::basic_ostream<std::byte>& bytesOutStream,
                         std::ostream& optLogOutStream) {
  auto dataConstructor = ael::ByteDataConstructor();

  auto ords = std::vector<std::uint64_t>{};

  constexpr auto barWidth = std::size_t{50};
  auto progressBar =
      ProgressBar(barWidth, wordsCount, "Decoding", optLogOutStream);
  ael::ArithmeticDecoder(decoded, bitsCount)
      .decode(dict, std::back_inserter(ords), wordsCount,
              progressBar.getTick());

  WordPacker::process(ords, dataConstructor, symBitLen);

  // std::copy(decoded.getEndBitsIter() - tailSize, decoded.getEndBitsIter(), //
  // TODO
  //           dataConstructor.getBitBackInserter());

  bytesOutStream.write(dataConstructor.data(), dataConstructor.size());
}

#endif  // APPLIB_DECODE_IMPL_HPP
