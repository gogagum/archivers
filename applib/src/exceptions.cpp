#include <applib/exceptions.hpp>
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