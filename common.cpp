#include "common.hpp"
#include "opt_ostream_ref.hpp"
#include <iostream>
#include <boost/format.hpp>

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
UnsupportedEncodeBitsMode::UnsupportedEncodeBitsMode(std::uint16_t numBits) :
    std::invalid_argument((
        boost::format("Encoding with %1% bit length is not supported.") % numBits
    ).str()) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
UnsupportedDecodeBitsMode::UnsupportedDecodeBitsMode(std::uint16_t numBits) :
    std::invalid_argument((
        boost::format("Decodeing with %1% bits mode is not supported.") % numBits
    ).str()) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
InvalidStreamParam::InvalidStreamParam(const std::string& streamParam) :
    std::invalid_argument((
        boost::format("\"%1%\" is an invalid -log-stream/-l argument."
                      " Choose between \"stdout\", \"stderr\" "
                      "and \"off\".") % streamParam
    ).str()) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
FileOpener::FileOpener(const std::string& inFileName,
                       const std::string& outFileName,
                       optout::OptOstreamRef optOs)
        : _finData(_openInFile(inFileName, optOs)), _fout(outFileName) {
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
std::vector<std::byte>
FileOpener::_openInFile(const std::string& fileInName, optout::OptOstreamRef optOs) {
    std::ifstream fin{fileInName, std::ifstream::ate | std::ifstream::binary};
    if (!fin.is_open()) {
        throw std::runtime_error(
            (boost::format("Could not open file: \"%1%\"") % fileInName).str());
    }

    fin.unsetf(std::ios::skipws);  // Not to eat newlines.
    std::streampos finSize;
    fin.seekg(0, std::ios::end);
    finSize = fin.tellg();
    optOs << "File size: " << static_cast<std::size_t>(finSize) << ".";
    fin.seekg(0, std::ios::beg);

    auto ret = std::vector<std::byte>(finSize);
    fin.read(reinterpret_cast<char*>(ret.data()), finSize);
    return ret;
}

//----------------------------------------------------------------------------//
optout::OptOstreamRef get_out_stream(const std::string& arg) {
    optout::OptOstreamRef outStream;

    if (arg == "stdout") {
        outStream = std::cout;
    } else if (arg == "stderr") {
        outStream = std::cerr;
    } else if (arg == "off") {
    } else {
        throw InvalidStreamParam(arg);
    }

    return outStream;
}
