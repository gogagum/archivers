#ifndef ARITHMETIC_CODER_ENCODED_HPP
#define ARITHMETIC_CODER_ENCODED_HPP

#include <vector>
#include <array>
#include <cstdint>
#include <cstddef>

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

private:

    bool _moveBitFlag();

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
    if (auto& byteRef = *_data.rbegin(); bit) {
        byteRef |= _currBitFlag;
    } else {
        byteRef &= ~_currBitFlag;
    }
    if (_moveBitFlag()) {
        _data.push_back(std::byte());
    }
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

    static_assert(sizeof(SizeTBytes) == sizeof(T));

    auto& bytes = reinterpret_cast<SizeTBytes&>(s);
    for (auto byte: bytes) {
        putByte(byte);
    }
}

//----------------------------------------------------------------------------//
bool garchiever::ArithmeticCoderEncoded::_moveBitFlag() {
    if (_currBitFlag == std::byte{0b00000001}) {
        _currBitFlag = std::byte{0b10000000};
        return true;
    }
    _currBitFlag >>= 1;
    return false;
}

#endif // ARITHMETIC_CODER_ENCODED_HPP
