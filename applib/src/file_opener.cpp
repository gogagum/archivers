#include <applib/file_opener.hpp>

#include <fmt/format.h>

////////////////////////////////////////////////////////////////////////////////
FileOpener::FileOpener(const std::string& inFileName,
                       const std::string& outFileName,
                       optout::OptOstreamRef optOs)
        : _finData(_openInFile(inFileName, optOs)),
          _fout(outFileName, std::ios::binary) {
    if (!_fout.is_open()) {
        throw std::runtime_error(
            fmt::format("Could not open file: \"{}\"", outFileName));
    }
}

////////////////////////////////////////////////////////////////////////////////
std::span<const std::byte> FileOpener::getInData() const {
       return std::span<const std::byte>(_finData.cbegin(), _finData.cend());
}

////////////////////////////////////////////////////////////////////////////////
std::ofstream& FileOpener::getOutFileStream() {
    return _fout;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::byte>
FileOpener::_openInFile(const std::string& fileInName,
                        optout::OptOstreamRef optOs) {
    std::ifstream fin{fileInName, std::ifstream::ate | std::ifstream::binary};
    if (!fin.is_open()) {
        throw std::runtime_error(
            fmt::format("Could not open file: \"{}\"", fileInName));
    }

    fin.unsetf(std::ios::skipws);  // Not to eat newlines.
    std::streampos finSize;
    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    optOs << fmt::format("File size: {}.", finSize) << std::endl;
    fin.seekg(0, std::ios::beg);

    auto ret = std::vector<std::byte>(finSize);
    fin.read(reinterpret_cast<char*>(ret.data()), finSize);
    return ret;
}
