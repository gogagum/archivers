#include <boost/range/irange.hpp>
#include "data_parser.hpp"

namespace ga {
////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
DataParser::DataParser(std::span<std::byte> data)
    : _data(data),
      _dataIter{_data.begin()},
      _currBitFlag{0b10000000} { }

//----------------------------------------------------------------------------//
std::byte DataParser::takeByte() {
    if (_currBitFlag == std::byte{0b10000000}) {
        std::byte ret = *_dataIter;
        ++_dataIter;
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
    if (_dataIter == _data.end()) {
        return false;
    }
    bool ret = (*_dataIter & _currBitFlag) != std::byte{0};
    _moveBitFlag();
    return ret;
}

//----------------------------------------------------------------------------//
void DataParser::_moveBitFlag() {
    _currBitFlag >>= 1;
    if (_currBitFlag == std::byte{0b00000000}) {
        _currBitFlag = std::byte{0b10000000};
        ++_dataIter;
    }
}

//----------------------------------------------------------------------------//
std::size_t DataParser::getNumBytes() const {
    return _data.size();
}

//----------------------------------------------------------------------------//
void DataParser::seek(std::size_t bitsOffset) {
    _dataIter = _data.begin() + bitsOffset / 8;
    const auto inByteOffset = bitsOffset % 8;
    _currBitFlag = std::byte{0b10000000} >> inByteOffset;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
bool operator==(const DataParser& dp1, const DataParser& dp2) {
    return dp1._data.data() == dp2._data.data()
            && dp1._dataIter == dp2._dataIter;
}

}  // namespace ga
