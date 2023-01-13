#ifndef FILE_OPENER_HPP
#define FILE_OPENER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <span>

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

#endif // FILE_OPENER_HPP
