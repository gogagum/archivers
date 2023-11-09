#include <fmt/format.h>

#include <applib/file_opener.hpp>
#include <ostream>

////////////////////////////////////////////////////////////////////////////////
std::span<const std::byte> FileOpener::getInData() const {
  return {finData_.cbegin(), finData_.cend()};
}

////////////////////////////////////////////////////////////////////////////////
std::basic_ofstream<std::byte>& FileOpener::getOutFileStream() {
  return fout_;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::byte> FileOpener::openInFile_(const std::string& fileInName,
                                               std::ostream& optOs) {
  auto fin =
      std::ifstream{fileInName, std::ifstream::ate | std::ifstream::binary};
  if (!fin.is_open()) {
    throw std::runtime_error(
        fmt::format("Could not open file: \"{}\"", fileInName));
  }

  fin.unsetf(std::ios::skipws);  // Not to eat newlines.
  auto finSize = std::streampos();
  fin.seekg(0, std::ios::end);
  finSize = fin.tellg();
  optOs << fmt::format("File size: {}.\n", finSize);
  fin.seekg(0, std::ios::beg);

  auto ret = std::vector<std::byte>(finSize);
  fin.read(reinterpret_cast<char*>(ret.data()), finSize);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
FileOpener::FileOpener(const std::string& inFileName,
                       const std::string& outFileName, std::ostream& optOs)
    : finData_(openInFile_(inFileName, optOs)),
      fout_(outFileName, std::ios::binary) {
  if (!fout_.is_open()) {
    throw std::runtime_error(
        fmt::format("Could not open file: \"{}\"", outFileName));
  }
}