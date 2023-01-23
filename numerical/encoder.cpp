#include <iostream>
#include <boost/program_options.hpp>

#include "flow/bytes_word_flow.hpp"

#include "../file_opener.hpp"

namespace bpo = boost::program_options;

using ga::fl::BytesWordFlow;
using ga::w::BytesWord;

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
        auto inFileBytes = fileOpener.getInData();
        auto wordFlow = BytesWordFlow<BytesWord<1>>(inFileBytes);

        auto countsMap = std::map<BytesWord<1>::Ord, std::uint64_t>();

        for (auto word : wordFlow) {
            ++countsMap[BytesWord<1>::ord(word)];
        }

        std::uint32_t numWords = countsMap.size();
        fileOpener.getOutFileStream().write(reinterpret_cast<char*>(&numWords),
                                            sizeof(std::uint32_t));




    } catch (const std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return 2;
    }

}
