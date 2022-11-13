#ifndef BIT_SYMBOL_HPP
#define BIT_SYMBOL_HPP

#include <cstdint>
#include <array>
#include <cassert>

namespace garchiever {

template <class I> requires std::is_integral_v<I>
constexpr I ceil_div(I dividend, I divider) {
    return (dividend - 1) / divider + 1;
}

template <std::size_t numBits>
class BitSymbol {
public:
    BitSymbol(std::byte* ptr, std::size_t offset);

private:
    std::array<std::byte, ceil_div(numBits, sizeof(std::byte))> _data;
};

}

template <std::size_t numBits>
garchiever::BitSymbol<numBits>::BitSymbol(std::byte* ptr, std::size_t offset) {
    assert(offset < numBits);
    std::ptrdiff_t inByteOffset = offset % sizeof(std::byte);
    const std::size_t byteOffset = offset / sizeof(std::byte);

    for (std::size_t i = byteOffset; i < _data.size() + byteOffset; ++i) {
        _data[i] = ptr[i];
        _data[i] <<= inByteOffset;
        if (i < _data.size() - 1) {
            std::byte next = _data[i + 1];
            next >>= inByteOffset;
            _data[i] |= next;
        }
    }
}

#endif // BIT_SYMBOL_HPP
