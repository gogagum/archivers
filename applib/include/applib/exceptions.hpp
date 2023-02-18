#ifndef APPLIB_EXCEPTIONS_HPP
#define APPLIB_EXCEPTIONS_HPP

#include <stdexcept>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// \brief The UnsupportedBitsMode class
//
class UnsupportedEncodeBitsMode : public std::invalid_argument {
public:
    UnsupportedEncodeBitsMode(std::uint16_t bits);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The UnsupportedDecodeBitsMode class
///
class UnsupportedDecodeBitsMode : public std::invalid_argument {
public:
    UnsupportedDecodeBitsMode(std::uint16_t bits);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The InvalidStreamParam class
///
class InvalidStreamParam : public std::invalid_argument {
public:
    InvalidStreamParam(const std::string& streamParam);
};

#endif
