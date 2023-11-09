#ifndef APPLIB_FILE_OPENER_HPP
#define APPLIB_FILE_OPENER_HPP

#include <fmt/format.h>

#include <cstddef>
#include <fstream>
#include <ostream>
#include <span>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
/// \brief The FilesOpener class
///
class FileOpener {
 public:
  FileOpener(const FileOpener&) = delete;

  /**
   * Default move-constructor.
   */
  FileOpener(FileOpener&&) noexcept = default;

  /**
   * @brief FileOpener - opener constructor from two files names.
   * @param inFileName - input file name.
   * @param outFileName - output file name.
   * @param optOs - optional out stream.
   */
  FileOpener(const std::string& inFileName, const std::string& outFileName,
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
  std::vector<std::byte> openInFile_(const std::string& fileInName,
                                     std::ostream& logOs);

 private:
  std::vector<std::byte> finData_;
  std::ofstream fout_;
};

#endif  // APPLIB_FILE_OPENER_HPP
