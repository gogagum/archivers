#include "arithmetic_decoder_decoded.hpp"

namespace ga {
////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
ArithmeticDecoderDecoded::ArithmeticDecoderDecoded(
        std::vector<std::byte>&& data)
    : _data(std::move(data)),
      _bytesRead{0},
      _startedBits{false},
      _currBitFlag{0b10000000} { }

//----------------------------------------------------------------------------//
std::byte ArithmeticDecoderDecoded::takeByte() {
    if (_startedBits) {
        throw BytesAfterBitsException();
    }
    std::byte ret = _data[_bytesRead];
    ++_bytesRead;
    return ret;
}

//----------------------------------------------------------------------------//
bool ArithmeticDecoderDecoded::takeBit() {
    _startedBits = true;
    if (_bytesRead == _data.size()) {
        return false;
    }
    bool ret = (_data[_bytesRead] & _currBitFlag) != std::byte{0};
    _moveBitFlag();
    return ret;
}

//----------------------------------------------------------------------------//
void ArithmeticDecoderDecoded::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
        ++_bytesRead;
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
ArithmeticDecoderDecoded::BytesAfterBitsException::BytesAfterBitsException(
        ) : std::runtime_error("Can`t write bytes after bits.") { }

}  // namespace ga
