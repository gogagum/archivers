#include <applib/decode_impl.hpp>

#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <boost/program_options.hpp>
#include <optional>
#include <ostream>
#include <stdexcept>

#include <applib/log_stream_get.hpp>

namespace bpo = boost::program_options;

std::ostream DecodeImpl::nullOut{nullptr};

////////////////////////////////////////////////////////////////////////////////
auto DecodeImpl::configure(int argc, char* argv[]) -> ConfigureRet {
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
        
        auto& outStrem = LogStreamGet::getLogStream(logStreamParam);
        auto filesOpener = FileOpener(inFileName, outFileName, outStrem);
        auto decoded = ael::DataParser(filesOpener.getInData());

        return {
            outStrem,
            std::move(filesOpener),
            std::move(decoded)
        };
} 
