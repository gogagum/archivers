#ifndef DECODER_FILE_OPENER_HPP
#define DECODER_FILE_OPENER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

////////////////////////////////////////////////////////////////////////////////
/// \brief The FilesOpener class
///
class FilesOpener{
public:
    FilesOpener(std::string inFileName, std::string outFileName);

    std::span<const std::byte> getInData() const;

    std::ofstream& getOutstream();

private:
    void _openInFile(const std::string& fileInName);

    void _openOutFile(std::string& fileOutName);

private:
    std::vector<std::byte> _finData;
    std::ofstream _fout;
};

#endif // DECODER_FILE_OPENER_HPP
