#include "misc.hpp"

//----------------------------------------------------------------------------//
constexpr std::uint8_t garchiever::misc::ceilLog2(std::size_t num) {
    static const std::size_t t[6] = {
        0xFFFFFFFF00000000ull,
        0x00000000FFFF0000ull,
        0x000000000000FF00ull,
        0x00000000000000F0ull,
        0x000000000000000Cull,
        0x0000000000000002ull
      };

      std::uint8_t ret = (((num & (num - 1)) == 0) ? 0 : 1);
      std::uint8_t j = 32;

      for (int i = 0; i < 6; i++) {
        std::uint8_t k = (((x & t[i]) == 0) ? 0 : j);
        ret += k;
        x >>= k;
        j >>= 1;
      }

      return ret;
}
