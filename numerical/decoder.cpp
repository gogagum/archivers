#include <ael/byte_data_constructor.hpp>
#include <ael/data_parser.hpp>
#include <ael/numerical_decoder.hpp>
#include <applib/decode_impl.hpp>
#include <applib/file_opener.hpp>
#include <applib/progress_bar.hpp>
#include <boost/program_options.hpp>
#include <cstdint>
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
    constexpr auto barWidth = 50uz;
    auto wordsProgressBar =
        ProgressBar(barWidth, dictSize, "Decoding words", cfg.outStream);
    auto countsProgressBar =
        ProgressBar(barWidth, dictSize, "Decoding counts", cfg.outStream);
    auto contentProgressBar = ProgressBar(barWidth, contentWordsCnt,
                                          "Decoding content", cfg.outStream);
    constexpr auto maxOrd = std::uint64_t{256};
    ael::NumericalDecoder(cfg.decoded, dictSize, contentWordsCnt, totalBitsCnt)
        .decode(std::back_inserter(contentWordsOrds), maxOrd,
                wordsProgressBar.getTick(), countsProgressBar.getTick(),
                contentProgressBar.getTick());

    auto dataConstructor = ael::ByteDataConstructor();

    std::ranges::copy(
        contentWordsOrds | std::views::transform([](std::uint64_t i) {
          return static_cast<std::byte>(i);
        }),
        dataConstructor.getByteBackInserter());

    cfg.fileOpener.getOutFileStream().write(dataConstructor.data(),
                                            dataConstructor.size());

  } catch (const std::exception& error) {
    std::cerr << error.what();
    return 1;
  }

  return 0;
}
