#pragma once

#ifndef MISC_HPP
#define MISC_HPP

#include <ostream>
#include <bitset>
#include <cstddef>

namespace ga {
//----------------------------------------------------------------------------//
std::ostream& operator<< (std::ostream& os, std::byte b);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
std::ostream& operator<< (std::ostream& os, std::byte b) {
   return os << std::bitset<8>(std::to_integer<int>(b));
}

}  // namespace ga

#endif // MISC_HPP


