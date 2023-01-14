#include <iostream>
#include <boost/format.hpp>

#include "file_opener.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
FileOpener::FileOpener(const std::string& inFileName,
                       const std::string& outFileName)
        : _finData(_openInFile(inFileName)), _fout(outFileName) {
    if (!_fout.is_open()) {
        throw std::runtime_error(
            (boost::format("Could not open filr: \"%1%\"") % outFileName).str());
    }
}

//----------------------------------------------------------------------------//
std::span<const std::byte> FileOpener::getInData() const {
       return std::span<const std::byte>(_finData.cbegin(), _finData.cend());
}

//----------------------------------------------------------------------------//
std::ofstream& FileOpener::getOutFileStream() {
    return _fout;
}

//----------------------------------------------------------------------------//
std::vector<std::byte> FileOpener::_openInFile(const std::string& fileInName) {
    std::ifstream fin{fileInName, std::ifstream::ate | std::ifstream::binary};
    if (!fin.is_open()) {
        throw std::runtime_error(
            (boost::format("Could not open file: \"%1%\"") % fileInName).str());
    }

    fin.unsetf(std::ios::skipws);  // Not to eat newlines.
    std::streampos finSize;
    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    std::cerr << "File size: " << finSize << "." << std::endl;
    fin.seekg(0, std::ios::beg);

    auto ret = std::vector<std::byte>(finSize);
    fin.read(reinterpret_cast<char*>(ret.data()), finSize);
    return ret;
}

