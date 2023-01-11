#include <boost/range/irange.hpp>
#include "data_parser.hpp"

namespace ga {
////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
DataParser::DataParser(std::span<std::byte> data)
    : _data(data),
      _dataIter{_data.begin()},
      _inByteOffset(0) { }

//----------------------------------------------------------------------------//
std::byte DataParser::takeByte() {
    if (_inByteOffset == 0) {
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
    bool ret = (*_dataIter & _getByteFlag()) != std::byte{0};
    _moveInByteOffset();
    return ret;
}

//----------------------------------------------------------------------------//
void DataParser::_moveInByteOffset() {
    ++_inByteOffset;
    if (_inByteOffset == 8) {
        _inByteOffset = 0;
        ++_dataIter;
    }
}

//----------------------------------------------------------------------------//
std::byte DataParser::_getByteFlag() const {
    return std::byte{0b10000000} >> _inByteOffset;
}

//----------------------------------------------------------------------------//
std::size_t DataParser::getNumBytes() const {
    return _data.size();
}

//----------------------------------------------------------------------------//
std::size_t DataParser::getNumBits() const {
    return _data.size() * 8;
}

//----------------------------------------------------------------------------//
void DataParser::seek(std::size_t bitsOffset) {
    _dataIter = _data.begin() + bitsOffset / 8;
    _inByteOffset = bitsOffset % 8;
}

//----------------------------------------------------------------------------//
auto DataParser::getCurrPosBitsIter() -> BitsIterator {
    return BitsIterator(*this, (_dataIter - _data.begin()) * 8 + _inByteOffset);
}

//----------------------------------------------------------------------------//
auto DataParser::getEndBitsIter() -> BitsIterator {
    return BitsIterator(*this, _data.size() * 8);
}

//----------------------------------------------------------------------------//
auto DataParser::getCurrTailRange() -> boost::iterator_range<BitsIterator> {
    return boost::make_iterator_range(getCurrPosBitsIter(), getEndBitsIter());
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
bool operator==(const DataParser& dp1, const DataParser& dp2) {
    return dp1._data.data() == dp2._data.data()
            && dp1._dataIter == dp2._dataIter;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
bool DataParser::BitsIterator::dereference() const {
    _owner->seek(_bitsPosition);
    bool retBit = _owner->takeBit();
    return retBit;
}

//----------------------------------------------------------------------------//
bool DataParser::BitsIterator::equal(const type& other) const {
    return _owner == other._owner
        && _bitsPosition == other._bitsPosition;
}

}  // namespace ga
