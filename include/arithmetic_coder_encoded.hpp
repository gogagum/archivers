#ifndef ARITHMETIC_CODER_ENCODED_HPP
#define ARITHMETIC_CODER_ENCODED_HPP

#include <boost/range/adaptor/reversed.hpp>
#include <vector>
#include <iostream>
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

    /**
     * @brief putBit - add single bit in the end.
     * @param bit - a bit to set.
     */
    void putBit(bool bit);

    /**
     * @brief putBitsRepeat
     * @param bit
     * @param num
     */
    void putBitsRepeat(bool bit, std::size_t num);

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
    bool _startedBits;
};

}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
garchiever::ArithmeticCoderEncoded::ArithmeticCoderEncoded()
    : _currBitFlag{0b10000000},
      _startedBits{false} {}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::putBit(bool bit) {
    _startedBits = true;
    std::cerr << (bit ? 1 : 0);
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
void
garchiever::ArithmeticCoderEncoded::putBitsRepeat(bool bit, std::size_t num) {
    for (std::size_t i = 0; i < num; ++i) {
        putBit(bit);
    }
}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::putByte(std::byte b) {
    assert(!_startedBits && "Can`t write bytes after bits.");
    _data.push_back(b);
}

//----------------------------------------------------------------------------//
template <class T>
void garchiever::ArithmeticCoderEncoded::putT(T s) {
    using SizeTBytes = std::array<std::byte, sizeof(T)>;

    static_assert(sizeof(SizeTBytes) == sizeof(T), "Error with sizes.");

    auto& bytes = reinterpret_cast<SizeTBytes&>(s);
    for (auto byte: boost::adaptors::reverse(bytes)) {
        putByte(byte);
    }
}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticCoderEncoded::fillEnd(bool bit) {
    while (_currBitFlag != std::byte{0b10000000}) {
        putBit(bit);
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
