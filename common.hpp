#ifndef COMMON_HPP
#define COMMON_HPP

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <span>

#include <word/bytes_word.hpp>
#include <word/bits_word.hpp>
#include "opt_ostream_ref.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The UnsupportedBitsMode class
///
class UnsupportedEncodeBitsMode : public std::invalid_argument {
public:
    UnsupportedEncodeBitsMode(std::uint16_t bits);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The UnsupportedDecodeBitsMode class
///
class UnsupportedDecodeBitsMode : public std::invalid_argument {
public:
    UnsupportedDecodeBitsMode(std::uint16_t bits);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The InvalidStreamParam class
///
class InvalidStreamParam : public std::invalid_argument {
public:
    InvalidStreamParam(const std::string& streamParam);
};


////////////////////////////////////////////////////////////////////////////////
/// \brief The FilesOpener class
///
class FileOpener{
public:

    /**
     * @brief FileOpener - opener constructor from two files names.
     * @param inFileName - input file name.
     * @param outFileName - output file name.
     * @param optOs - optional out stream.
     */
    FileOpener(const std::string& inFileName,
               const std::string& outFileName,
               optout::OptOstreamRef optOs = std::nullopt);

    /**
     * @brief getInData - get input file data.
     * @return bytes array view.
     */
    std::span<const std::byte> getInData() const;

    /**
     * @brief getOutFileStream - get output stream reference.
     * @return
     */
    std::ofstream& getOutFileStream();

private:
    std::vector<std::byte>
    _openInFile(const std::string& fileInName, optout::OptOstreamRef optOs);

private:
    std::vector<std::byte> _finData;
    std::ofstream _fout;
};

//----------------------------------------------------------------------------//
optout::OptOstreamRef get_out_stream(const std::string& arg);

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes>
void packWordIntoData(const ga::w::BytesWord<_numBytes> word, auto& cntr) {
    word.bytesOut(cntr.getByteBackInserter());
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
void packWordIntoData(const ga::w::BitsWord<_numBits> word, auto& cntr) {
    word.bitsOut(cntr.getBitBackInserter());
}


#endif // COMMON_HPP
