#include <applib/exceptions.hpp>
#include <fmt/format.h>

////////////////////////////////////////////////////////////////////////////////
UnsupportedEncodeBitsMode::UnsupportedEncodeBitsMode(std::uint16_t numBits) :
    std::invalid_argument(
        fmt::format("Encoding with {} bit length is not supported.", numBits)
    ) {}

////////////////////////////////////////////////////////////////////////////////
UnsupportedDecodeBitsMode::UnsupportedDecodeBitsMode(std::uint16_t numBits) :
    std::invalid_argument(
        fmt::format("Decodeing with {} bits mode is not supported.", numBits)
    ) {}

////////////////////////////////////////////////////////////////////////////////
InvalidStreamParam::InvalidStreamParam(const std::string& streamParam) :
    std::invalid_argument(
        fmt::format("\"{}\" is an invalid -log-stream/-l argument."
                    " Choose between \"stdout\", \"stderr\" "
                    "and \"off\".", streamParam
    )) {}
