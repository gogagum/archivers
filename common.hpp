#ifndef COMMON_HPP
#define COMMON_HPP

#include <stdexcept>
#include <fstream>

#include "byte_data_constructor.hpp"

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseAAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
class BaseAdaptiveEncodeImpl {
protected:
    static void processImpl(auto& fileOpener,
                            auto&& tail,
                            auto& coder) {
        auto encoded = ga::ByteDataConstructor();
        encoded.putT<std::uint16_t>(bitsNum);
        encoded.putT<std::uint16_t>(tail.size());
        const auto wordsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        const auto bitsCountPos = encoded.saveSpaceForT<std::uint64_t>();
        auto [wordsCount, bitsCount] = coder.encode(encoded);
        encoded.putTToPosition<std::uint64_t>(wordsCount, wordsCountPos);
        encoded.putTToPosition<std::uint64_t>(bitsCount, bitsCountPos);
        std::copy(tail.begin(), tail.end(), encoded.getBitBackInserter());
        fileOpener.getOutFileStream().write(encoded.data<char>(), encoded.size());
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The UnsupportedBitsMode class
///
class UnsupportedEncodeBitsMode : std::invalid_argument {
public:
    UnsupportedEncodeBitsMode(std::uint16_t bits);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The UnsupportedDecodeBitsMode class
///
class UnsupportedDecodeBitsMode : std::invalid_argument {
public:
    UnsupportedDecodeBitsMode(std::uint16_t bits);
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
     */
    FileOpener(const std::string& inFileName, const std::string& outFileName);

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
    std::vector<std::byte> _openInFile(const std::string& fileInName);

private:
    std::vector<std::byte> _finData;
    std::ofstream _fout;
};

#endif // COMMON_HPP
