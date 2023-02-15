#ifndef COMMON_HPP
#define COMMON_HPP

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <span>

#include <word/bytes_word.hpp>
#include <word/bits_word.hpp>
#include <boost/container/static_vector.hpp>
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

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAAdaptiveEncodeImpl class
///
template<template<std::uint16_t> class Flow, template<std::uint16_t> class Word>
class OrdAndTailSplitter {
public:
    struct Ret {
        std::vector<std::uint64_t> ords;
        boost::container::static_vector<bool, 32> tail;
    };

public:
    static Ret process(const std::span<const std::byte>& inData,
                       std::uint8_t numBits) {
        #define FILE_SPLITTER_BITS_CASE(bits) \
            case (bits): return _process<bits>(inData);

        switch (numBits) {
            FILE_SPLITTER_BITS_CASE(8);
            FILE_SPLITTER_BITS_CASE(9);
            FILE_SPLITTER_BITS_CASE(10);
            FILE_SPLITTER_BITS_CASE(11);
            FILE_SPLITTER_BITS_CASE(12);
            FILE_SPLITTER_BITS_CASE(13);
            FILE_SPLITTER_BITS_CASE(14);
            FILE_SPLITTER_BITS_CASE(15);
            FILE_SPLITTER_BITS_CASE(16);
            FILE_SPLITTER_BITS_CASE(17);
            FILE_SPLITTER_BITS_CASE(18);
            FILE_SPLITTER_BITS_CASE(19);
            FILE_SPLITTER_BITS_CASE(20);
            FILE_SPLITTER_BITS_CASE(21);
            FILE_SPLITTER_BITS_CASE(22);
            FILE_SPLITTER_BITS_CASE(23);
            FILE_SPLITTER_BITS_CASE(24);
            FILE_SPLITTER_BITS_CASE(25);
            FILE_SPLITTER_BITS_CASE(26);
            FILE_SPLITTER_BITS_CASE(27);
            FILE_SPLITTER_BITS_CASE(28);
            FILE_SPLITTER_BITS_CASE(29);
            FILE_SPLITTER_BITS_CASE(30);
            FILE_SPLITTER_BITS_CASE(31);
            FILE_SPLITTER_BITS_CASE(32);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }

        #undef FILE_SPLITTER_BITS_CASE
    }

private:
    template <std::uint8_t bitsNum>
    static Ret _process(const std::span<const std::byte>& inData) {
        auto flow = Flow<bitsNum>(inData);
        std::vector<std::uint64_t> retOrds;
        auto outIter = std::back_inserter(retOrds);
        std::transform(flow.begin(), flow.end(), outIter,
                       [](const Word<bitsNum>& w) {
                           return Word<bitsNum>::ord(w);
                       });
        auto flowTail = flow.getTail();
        auto retTail = boost::container::static_vector<bool, 32>(
                    flowTail.begin(), flowTail.end());
        return { retOrds, retTail };
    }
};

#endif // COMMON_HPP
