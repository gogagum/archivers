#include "decoder_file_opener.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
FilesOpener::FilesOpener(std::string inFileName, std::string outFileName) {
    _openInFile(inFileName);
    _openOutFile(outFileName);
}

//----------------------------------------------------------------------------//
std::span<const std::byte> FilesOpener::getInData() const {
       return std::span<const std::byte>(_finData.cbegin(), _finData.cend());
}

//----------------------------------------------------------------------------//
std::ofstream& FilesOpener::getOutstream() {
    return _fout;
}

//----------------------------------------------------------------------------//
void FilesOpener::_openInFile(const std::string& fileInName) {
    std::ifstream fin{fileInName, std::ifstream::ate | std::ifstream::binary};
    if (!fin.is_open()) {
        std::cout << "Could not open file: " << fileInName << std::endl;
        // TODO throw
    }
    fin.unsetf(std::ios::skipws);
    // get its size:
    std::streampos finSize;

    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    std::cerr << "File size: " << finSize << "." << std::endl;
    fin.seekg(0, std::ios::beg);

    _finData.resize(finSize);

    fin.read(reinterpret_cast<char*>(_finData.data()), finSize);
}

//----------------------------------------------------------------------------//
void FilesOpener::_openOutFile(std::string& fileOutName) {
    _fout.open(fileOutName, std::ios::binary | std::ios::out);
    if (!_fout.is_open()) {
        std::cout << "Could not open file: " << fileOutName << "." << std::endl;
        // TODO throw
    }
}
