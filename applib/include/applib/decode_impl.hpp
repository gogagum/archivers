#ifndef APPLIB_DECODE_IMPL_HPP
#define APPLIB_DECODE_IMPL_HPP

#include "ael/data_parser.hpp"
#include "applib/opt_ostream.hpp"
#include <cstddef>
#include <ostream>
#include <vector>
#include <cstdint>

#include <boost/program_options.hpp>

#include <ael/byte_data_constructor.hpp>
#include <ael/arithmetic_decoder.hpp>
#include <ael/data_parser.hpp>

#include <applib/word_packer.hpp>
#include <applib/file_opener.hpp>

namespace {

namespace bpo = boost::program_options;

struct DecodeImpl {
    struct ConfigureRet {
        optout::OptOstreamRef outStream;
        FileOpener fileOpener;
        ael::DataParser decoded;
    };

    static ConfigureRet configure(int argc, char* argv[]) {
        bpo::options_description appOptionsDescr("Console options.");

        std::string inFileName;
        std::string outFileName;
        std::string logStreamParam;

        appOptionsDescr.add_options() (
            "input-file,i",
            bpo::value(&inFileName)->required(),
            "In file name."
        ) (
            "out-filename,o",
            bpo::value(&outFileName)->default_value({}),
            "Out file name."
        ) (
            "log-stream,l",
            bpo::value(&logStreamParam)->default_value("stdout"),
            "Log stream."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        outFileName = outFileName.empty()
            ? inFileName + "-decoded"
            : outFileName;

        auto outStream = get_out_stream(logStreamParam);
        auto filesOpener = FileOpener(inFileName, outFileName, outStream);
        auto decoded = ael::DataParser(filesOpener.getInData());

        return {
            outStream,
            std::move(filesOpener),
            std::move(decoded)
        };
    } 

    static void process(ael::DataParser& decoded,
                 auto& dict,
                 std::size_t wordsCount,
                 std::size_t bitsCount,
                 std::uint16_t symBitLen,
                 std::uint16_t tailSize,
                 std::ostream& bytesOutStream,
                 optout::OptOstreamRef optLogOutStream) {
    auto dataConstructor = ael::ByteDataConstructor();

    std::deque<std::uint64_t> ords;

    ael::ArithmeticDecoder::decode(
        decoded, dict, std::back_inserter(ords),
        wordsCount, bitsCount, optLogOutStream);

    WordPacker::process(ords, dataConstructor, symBitLen);

    std::copy(decoded.getEndBitsIter() - tailSize, decoded.getEndBitsIter(),
              dataConstructor.getBitBackInserter());

    bytesOutStream.write(dataConstructor.data<char>(), dataConstructor.size());

}

};

}

#endif