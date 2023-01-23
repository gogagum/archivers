#include <boost/range/irange.hpp>

#include "byte_data_constructor.hpp"
#include "bits_iterator.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
ByteDataConstructor::ByteDataConstructor()
    : _currBitFlag{0b10000000} {}

//----------------------------------------------------------------------------//
void ByteDataConstructor::putBit(bool bit) {
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
    if (_currBitFlag == std::byte{0b10000000}) {
        _data.push_back(b);
    } else {
        for (auto bit: impl::make_bits_iterator_range(b)) {
            putBit(bit);
        }
    }
}

//----------------------------------------------------------------------------//
std::size_t ByteDataConstructor::size() const {
    return _data.size();
}

//----------------------------------------------------------------------------//
auto ByteDataConstructor::getBitBackInserter() -> BitBackInserter {
    return BitBackInserter(*this);
}

//----------------------------------------------------------------------------//
auto ByteDataConstructor::getByteBackInserter() -> ByteBackInserter {
    return ByteBackInserter(*this);
}

//----------------------------------------------------------------------------//
std::size_t ByteDataConstructor::saveBytesSpace(std::size_t bytesCnt) {
    if (_currBitFlag != std::byte{0b10000000}) {
        throw BytesAfterBits("Tried saving bytes with bit flag != 0b10000000.");
    }
    auto ret = _data.size();
    _data.resize(_data.size() + bytesCnt);
    return ret;
}

//----------------------------------------------------------------------------//
void ByteDataConstructor::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
    }
}

}  // namespace ga
