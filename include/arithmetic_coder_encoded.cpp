#include <boost/range/irange.hpp>

#include "arithmetic_coder_encoded.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
ArithmeticCoderEncoded::ArithmeticCoderEncoded()
    : _currBitFlag{0b10000000},
      _startedBits{false} {}

//----------------------------------------------------------------------------//
void ArithmeticCoderEncoded::putBit(bool bit) {
    _startedBits = true;
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
void ArithmeticCoderEncoded::putBitsRepeat(bool bit, std::size_t num) {
    for ([[maybe_unused]] auto _ : boost::irange<std::size_t>(0, num)) {
        putBit(bit);
    }
}

//----------------------------------------------------------------------------//
void
ArithmeticCoderEncoded::putBitsRepeatWithReset(bool bit, std::size_t& num) {
    putBitsRepeat(bit, num);
    num = 0;
}

//----------------------------------------------------------------------------//
void ArithmeticCoderEncoded::putByte(std::byte b) {
    assert(!_startedBits && "Can`t write bytes after bits.");
    _data.push_back(b);
}

//----------------------------------------------------------------------------//
const std::byte* ArithmeticCoderEncoded::data() const {
    return _data.data();
}

//----------------------------------------------------------------------------//
const std::size_t ArithmeticCoderEncoded::bytesSize() const {
    return _data.size();
}

//----------------------------------------------------------------------------//
void ArithmeticCoderEncoded::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
    }
}

}  // namespace ga
