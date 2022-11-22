#pragma once

#ifndef ARITHMETIC_DECODER_DECODED_HPP
#define ARITHMETIC_DECODER_DECODED_HPP

#include <boost/range/adaptor/reversed.hpp>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <array>
#include <ranges>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoderDecoded class
///
class ArithmeticDecoderDecoded {
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BytesAfterBitsException class
    ///
    class BytesAfterBitsException : public std::runtime_error {
    public:
        BytesAfterBitsException();
    };

public:

    ArithmeticDecoderDecoded(std::vector<std::byte>&& data);

    std::byte takeByte();

    bool takeBit();

    template <class T>
    T takeT();

    uint8_t getSymNumBytes() const;

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::size_t _bytesRead;
    bool _startedBits;
    std::byte _currBitFlag;
};

}  //

#endif // ARITHMETIC_DECODER_DECODED_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
garchiever::ArithmeticDecoderDecoded::ArithmeticDecoderDecoded(
        std::vector<std::byte>&& data)
    : _data(std::move(data)),
      _bytesRead{0},
      _startedBits{false},
      _currBitFlag{0b10000000} { }

//----------------------------------------------------------------------------//
std::byte garchiever::ArithmeticDecoderDecoded::takeByte() {
    if (_startedBits) {
        throw BytesAfterBitsException();
    }
    std::byte ret = _data[_bytesRead];
    ++_bytesRead;
    return ret;
}

//----------------------------------------------------------------------------//
bool garchiever::ArithmeticDecoderDecoded::takeBit() {
    _startedBits = true;
    if (_bytesRead == _data.size()) {
        return false;
    }
    bool ret = (_data[_bytesRead] & _currBitFlag) != std::byte{0};
    _moveBitFlag();
    std::cerr << (ret ? 1 : 0);
    return ret;
}

//----------------------------------------------------------------------------//
template <class T>
T garchiever::ArithmeticDecoderDecoded::takeT() {
    using TBytes = std::array<std::byte, sizeof(T)>;

    static_assert(sizeof(TBytes) == sizeof(T), "Error with sizes.");

    auto ret = T();
    auto& bytes = reinterpret_cast<TBytes&>(ret);
    for (auto& byte: boost::adaptors::reverse(bytes)) {
        byte = takeByte();
    }
    return ret;
}

//----------------------------------------------------------------------------//
void garchiever::ArithmeticDecoderDecoded::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
        ++_bytesRead;
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
garchiever::ArithmeticDecoderDecoded::BytesAfterBitsException::BytesAfterBitsException(
        ) : std::runtime_error("Can`t write bytes after bits.") { }
