#ifndef APPLIB_FILE_OPENER_HPP
#define APPLIB_FILE_OPENER_HPP

#include <cstddef>
#include <ostream>
#include <vector>
#include <string>
#include <span>
#include <fstream>

#include <fmt/format.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief The FilesOpener class
///
class FileOpener{
public:

    /**
     * Default move-constructor.
     */
    FileOpener(FileOpener&&) = default;

    /**
     * @brief FileOpener - opener constructor from two files names.
     * @param inFileName - input file name.
     * @param outFileName - output file name.
     * @param optOs - optional out stream.
     */
    FileOpener(const std::string& inFileName,
               const std::string& outFileName,
               std::ostream& logOs);

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

    std::vector<std::byte> _openInFile(const std::string& fileInName,
                                       std::ostream& logOs);

private:
    std::vector<std::byte> _finData;
    std::ofstream _fout;
};

#endif  // APPLIB_FILE_OPENER_HPP
