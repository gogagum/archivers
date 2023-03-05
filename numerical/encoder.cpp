#include <iostream>
#include <iterator>
#include <ranges>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <ael/numerical_coder.hpp>
#include <ael/flow/bytes_word_flow.hpp>
#include <ael/dictionary/decreasing_counts_dictionary.hpp>
#include <ael/dictionary/decreasing_on_update_dictionary.hpp>

#include <applib/opt_ostream.hpp>
#include <applib/ord_and_tail_splitter.hpp>
#include <applib/file_opener.hpp>

namespace bpo = boost::program_options;

using ael::fl::BytesWordFlow;
using ael::w::BytesWord;

using CountsDict = ael::dict::DecreasingCountDictionary<std::uint64_t>;
using DictWordsDict = ael::dict::DecreasingOnUpdateDictionary;
using ContentDict = ael::dict::DecreasingOnUpdateDictionary;

using DictWordsFlow = std::vector<BytesWord<1>>;

int main(int argc, char* argv[]) {
    bpo::options_description appOptionsDescr("Console options.");

    std::string inFileName;
    std::string outFileName;
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
            "log-stream,l",
            bpo::value(&logStreamParam)->default_value("stdout"),
            "Log stream."
        );

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, appOptionsDescr), vm);
        bpo::notify(vm);

        outFileName = outFileName.empty() ? inFileName + "-encoded" : outFileName;
        optout::OptOstreamRef outStream = get_out_stream(logStreamParam);
        auto fileOpener = FileOpener(inFileName, outFileName, outStream);
        auto inFileBytes = fileOpener.getInData();
        auto wordFlow = BytesWordFlow<1>(inFileBytes);

        auto ordFlow = std::vector<std::uint64_t>();

        std::transform(wordFlow.begin(), wordFlow.end(), 
                       std::back_inserter(ordFlow),
                       [](const auto& word) {
                           return BytesWord<1>::ord(word);
                       });

        auto dataConstructor = ael::ByteDataConstructor();        

        const auto dictSizePos = dataConstructor.saveSpaceForT<std::uint64_t>();
        const auto wordsBitsCntPos = dataConstructor.saveSpaceForT<std::uint64_t>();
        const auto wordsCountsBitsCntPos = dataConstructor.saveSpaceForT<std::uint64_t>();
        dataConstructor.putT<std::uint64_t>(ordFlow.size());
        const auto contentBitsCntPos = dataConstructor.saveSpaceForT<std::uint64_t>();

        auto layoutInfo =
             ael::NumericalCoder::encode(ordFlow, dataConstructor, outStream);

        dataConstructor.putTToPosition(layoutInfo.dictSize,
                                       dictSizePos);
        dataConstructor.putTToPosition(layoutInfo.wordsBitsCnt,
                                       wordsBitsCntPos);                               
        dataConstructor.putTToPosition(layoutInfo.wordsCountsBitsCnt,
                                       wordsCountsBitsCntPos);
        dataConstructor.putTToPosition(layoutInfo.contentBitsCnt,
                                       contentBitsCntPos);

        fileOpener.getOutFileStream().write(dataConstructor.data<char>(),
                                            dataConstructor.size());

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

}
