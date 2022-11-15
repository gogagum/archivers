#ifndef ARITHMETIC_CODER_ENCODED_HPP
#define ARITHMETIC_CODER_ENCODED_HPP

#include <vector>
#include <array>
#include <cstdint>
#include <cstddef>
#include <ranges>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoderEncoded class
///
class ArithmeticCoderEncoded {
public:
    ArithmeticCoderEncoded();

    void putBit(bool bit);

    void putByte(std::byte b);

    template <class T>
    void putT(T s);

    void fillEnd(bool bit);

    const std::byte* data() const;

    const std::size_t bytesSize() const;

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::byte _currBitFlag;
};

}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
garchiever::ArithmeticCoderEncoded::ArithmeticCoderEncoded()
    : _currBitFlag{0b10000000} {}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::putBit(bool bit) {
    if (_currBitFlag == std::byte{0b10000000}) {
        _data.push_back(std::byte{0b00000000});
    }
    if (bit) {
        *_data.rbegin() |= _currBitFlag;
    } else {
        *_data.rbegin() &= ~_currBitFlag;
    }
    _moveBitFlag();
}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::putByte(std::byte b) {
    if (_currBitFlag == std::byte{0b10000000}) {
        _data.push_back(b);
    } else {
        for (std::uint8_t i = 0; i < 8; ++i) {
            auto currBit = bool((b & std::byte{0b10000000}) >> 7);
            putBit(currBit);
            b <<= 1;
        }
    }
}

//----------------------------------------------------------------------------//
template <class T>
void garchiever::ArithmeticCoderEncoded::putT(T s) {
    using SizeTBytes = std::array<std::byte, sizeof(T)>;

    static_assert(sizeof(SizeTBytes) == sizeof(T), "Error with sizes.");

    auto& bytes = reinterpret_cast<SizeTBytes&>(s);
    for (auto byte: std::ranges::reverse_view(bytes)) {
        putByte(byte);
    }
}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::fillEnd(bool bit) {
    if (_currBitFlag != std::byte{0b10000000}) {
        while (_currBitFlag != std::byte{0b00000000}) {
            putBit(bit);
        }
        _currBitFlag = std::byte{0b10000000};
    }
}

//----------------------------------------------------------------------------//
const std::byte* garchiever::ArithmeticCoderEncoded::data() const {
    return _data.data();
}

//----------------------------------------------------------------------------//
const std::size_t garchiever::ArithmeticCoderEncoded::bytesSize() const {
    return _data.size();
}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
    }
}

#endif // ARITHMETIC_CODER_ENCODED_HPP
