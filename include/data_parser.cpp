#include <boost/range/irange.hpp>
#include "data_parser.hpp"

namespace ga {
////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
DataParser::DataParser(
        std::vector<std::byte>&& data)
    : _data(std::move(data)),
      _bytesRead{0},
      _currBitFlag{0b10000000} { }

//----------------------------------------------------------------------------//
std::byte DataParser::takeByte() {
    if (_currBitFlag == std::byte{0b10000000}) {
        std::byte ret = _data[_bytesRead];
        ++_bytesRead;
        return ret;
    }
    auto ret = std::byte{0};
    for ([[maybe_unused]] std::size_t _: boost::irange(0, 8)) {
        ret <<= 1;
        ret |= takeBit() ? std::byte{0b00000001} : std::byte{0b00000000};
    }
    return ret;
}

//----------------------------------------------------------------------------//
bool DataParser::takeBit() {
    if (_bytesRead == _data.size()) {
        return false;
    }
    bool ret = (_data[_bytesRead] & _currBitFlag) != std::byte{0};
    _moveBitFlag();
    return ret;
}

//----------------------------------------------------------------------------//
void DataParser::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
        ++_bytesRead;
    }
}

}  // namespace ga
