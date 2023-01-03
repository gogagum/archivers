#include <boost/range/irange.hpp>

#include "byte_data_constructor.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
ByteDataConstructor::ByteDataConstructor()
    : _currBitFlag{0b10000000},
      _startedBits{false} {}

//----------------------------------------------------------------------------//
void ByteDataConstructor::putBit(bool bit) {
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
void ByteDataConstructor::putBitsRepeat(bool bit, std::size_t num) {
    for ([[maybe_unused]] auto _ : boost::irange<std::size_t>(0, num)) {
        putBit(bit);
    }
}

//----------------------------------------------------------------------------//
void
ByteDataConstructor::putBitsRepeatWithReset(bool bit, std::size_t& num) {
    putBitsRepeat(bit, num);
    num = 0;
}

//----------------------------------------------------------------------------//
void ByteDataConstructor::putByte(std::byte b) {
    assert(!_startedBits && "Can`t write bytes after bits.");
    _data.push_back(b);
}

//----------------------------------------------------------------------------//
const std::byte* ByteDataConstructor::data() const {
    return _data.data();
}

//----------------------------------------------------------------------------//
const std::size_t ByteDataConstructor::bytesSize() const {
    return _data.size();
}


//----------------------------------------------------------------------------//
auto ByteDataConstructor::getBitBackInserter() -> BitBackInserter {
    return BitBackInserter(*this);
}

//----------------------------------------------------------------------------//
void ByteDataConstructor::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
    }
}

}  // namespace ga
