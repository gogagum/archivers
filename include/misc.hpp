#pragma once

#ifndef MISC_HPP
#define MISC_HPP

#include <ostream>
#include <bitset>
#include <cstddef>

namespace garchiever {
//----------------------------------------------------------------------------//
std::ostream& operator<< (std::ostream& os, std::byte b);

}

#endif // MISC_HPP

//----------------------------------------------------------------------------//
std::ostream& garchiever::operator<< (std::ostream& os, std::byte b) {
   return os << std::bitset<8>(std::to_integer<int>(b));
}
