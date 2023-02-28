#include <ael/impl/multiply_and_divide.hpp>

namespace ael::impl {

////////////////////////////////////////////////////////////////////////////////
std::uint64_t multiply_and_divide(std::uint64_t l,
                                  std::uint64_t r,
                                  std::uint64_t d) {
    return (
            bmp::uint128_t{l} * bmp::uint128_t{r} / d
        ).convert_to<std::uint64_t>();
}

////////////////////////////////////////////////////////////////////////////////
std::uint64_t multiply_decrease_and_divide(std::uint64_t l,
                                           std::uint64_t r,
                                           std::uint64_t d) {
    return (
            (bmp::uint128_t{l} * bmp::uint128_t{r} - 1) / d
        ).convert_to<std::uint64_t>();
}

}