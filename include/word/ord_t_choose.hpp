#ifndef ORD_T_CHOOSE_HPP
#define ORD_T_CHOOSE_HPP

#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>

namespace ga::w::impl {

template <std::uint16_t numBits>
struct OrdTChoose;

template <std::uint16_t numBits> requires (numBits < 64)
struct OrdTChoose<numBits> {
    using Type = std::uint64_t;
};

template <std::uint16_t numBits> requires (numBits >= 64 && numBits < 128)
struct OrdTChoose<numBits> {
    using Type = boost::multiprecision::uint128_t;
};

template <std::uint16_t numBits> requires (numBits >= 128 && numBits < 256)
struct OrdTChoose<numBits> {
    using Type = boost::multiprecision::uint256_t;
};

template <std::uint16_t numBits> requires (numBits >= 256 && numBits < 512)
struct OrdTChoose<numBits> {
    using Type = boost::multiprecision::uint512_t;
};

template <std::uint16_t numBits> requires (numBits >= 512 && numBits < 1024)
struct OrdTChoose<numBits> {
    using Type = boost::multiprecision::uint1024_t;
};

}


#endif // ORD_T_CHOOSE_HPP
