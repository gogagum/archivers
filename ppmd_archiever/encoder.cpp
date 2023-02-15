#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>

#include "../common.hpp"
#include "encoder_impl.hpp"

#define BITS_CASE(bits, fileOpener, contextLength, dict, outStream) \
    case (bits): PPMDEncodeImpl<bits>::process((fileOpener), (contextLength), (dict), (outStream)); break;

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
    std::uint16_t numBits;
    std::uint16_t contextLength;
    std::string logStreamParam;

    try {
        appOptionsDescr.add_options() (
                "input-file,i",
                bpo::value(&inFileName)->required(),
                "In file name."
            ) (
                "out-filename,o",
                bpo::value(&outFileName)->default_value({}),
                "Out file name."
            ) (
                "bits,b",
                bpo::value(&numBits)->default_value(16),
                "Word bits count."
            ) (
                "context-length,c",
                bpo::value(&contextLength)->default_value(4),
                "Context length."
            ) (
                "log-stream,l",
                bpo::value(&logStreamParam)->default_value("stdout"),
                "Log stream."
            );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        if (contextLength > 16) {
            throw std::logic_error("Context length is not supported.");
        }

        outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
        optout::OptOstreamRef outStream = get_out_stream(logStreamParam);
        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
        auto dict = Dict(1ull << numBits);

        switch (numBits) {
            BITS_CASE(8, fileOpener, contextLength, dict, outStream);
            BITS_CASE(9, fileOpener, contextLength, dict, outStream);
            BITS_CASE(10, fileOpener, contextLength, dict, outStream);
            BITS_CASE(11, fileOpener, contextLength, dict, outStream);
            BITS_CASE(12, fileOpener, contextLength, dict, outStream);
            BITS_CASE(13, fileOpener, contextLength, dict, outStream);
            BITS_CASE(14, fileOpener, contextLength, dict, outStream);
            BITS_CASE(15, fileOpener, contextLength, dict, outStream);
            BITS_CASE(16, fileOpener, contextLength, dict, outStream);
            BITS_CASE(17, fileOpener, contextLength, dict, outStream);
            BITS_CASE(18, fileOpener, contextLength, dict, outStream);
            BITS_CASE(19, fileOpener, contextLength, dict, outStream);
            BITS_CASE(20, fileOpener, contextLength, dict, outStream);
            BITS_CASE(21, fileOpener, contextLength, dict, outStream);
            BITS_CASE(22, fileOpener, contextLength, dict, outStream);
            BITS_CASE(23, fileOpener, contextLength, dict, outStream);
            BITS_CASE(24, fileOpener, contextLength, dict, outStream);
            BITS_CASE(25, fileOpener, contextLength, dict, outStream);
            BITS_CASE(26, fileOpener, contextLength, dict, outStream);
            BITS_CASE(27, fileOpener, contextLength, dict, outStream);
            BITS_CASE(28, fileOpener, contextLength, dict, outStream);
            BITS_CASE(29, fileOpener, contextLength, dict, outStream);
            BITS_CASE(30, fileOpener, contextLength, dict, outStream);
            BITS_CASE(31, fileOpener, contextLength, dict, outStream);
            BITS_CASE(32, fileOpener, contextLength, dict, outStream);
        default:
            throw UnsupportedEncodeBitsMode(numBits); break;
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}
