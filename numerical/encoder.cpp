#include <iostream>
#include <boost/program_options.hpp>

#include "../file_opener.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;

    try {
        appOptionsDescr.add_options()
            ("input-file,i", bpo::value(&inFileName)->required(), "In file name.")
            ("out-filename,o", bpo::value(&outFileName)->default_value(inFileName + "-out"), "Out file name.");

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);
    } catch (const std::logic_error& error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    try {
        auto fileOpener = FileOpener(inFileName, outFileName);





    } catch (const std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return 2;
    }

}
