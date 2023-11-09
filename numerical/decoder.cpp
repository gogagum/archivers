#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/numerical_decoder.hpp>
#include <applib/decode_impl.hpp>
#include <applib/file_opener.hpp>
#include <boost/program_options.hpp>
#include <cstdint>
#include <indicators/progress_bar.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
  try {
    auto cfg = DecodeImpl::configure(argc, argv);

    const auto takeWithLog = [&]<class T>(const std::string& msg, T) {
      const auto ret = cfg.decoded.takeT<T>();
      cfg.outStream << msg << static_cast<std::int64_t>(ret) << "\n";
      return ret;
    };

    const auto dictSize = takeWithLog("Dictionary size: ", std::uint64_t{});
    const auto contentWordsCnt =
        takeWithLog("Content words number: ", std::uint64_t{});
    const auto totalBitsCnt =
        takeWithLog("Bits for content decoding: ", std::uint64_t{});

    auto contentWordsOrds = std::vector<std::uint64_t>();
    auto wordsProgressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{dictSize},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Decoding words"},
        indicators::option::Stream{cfg.outStream});
    auto countsProgressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{dictSize},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Decoding counts"},
        indicators::option::Stream{cfg.outStream});
    auto contentProgressBar = indicators::ProgressBar(
        indicators::option::BarWidth{50},
        indicators::option::MaxProgress{contentWordsCnt},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{"Decoding content"},
        indicators::option::Stream{cfg.outStream});
    ael::NumericalDecoder(cfg.decoded, dictSize, contentWordsCnt, totalBitsCnt)
        .decode(
            std::back_inserter(contentWordsOrds), 256,
            [&wordsProgressBar] {
              wordsProgressBar.tick();
            },
            [&countsProgressBar] {
              countsProgressBar.tick();
            },
            [&contentProgressBar] {
              contentProgressBar.tick();
            });

    auto dataConstructor = ael::ByteDataConstructor();

    std::ranges::copy(
        contentWordsOrds | std::views::transform([](std::uint64_t i) {
          return static_cast<std::byte>(i);
        }),
        dataConstructor.getByteBackInserter());

    cfg.fileOpener.getOutFileStream().write(
        reinterpret_cast<const char*>(dataConstructor.data()),
        dataConstructor.size());

  } catch (const std::exception& error) {
    std::cerr << error.what();
    return 1;
  }

  return 0;
}
