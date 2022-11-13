#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "bytes_symbol.hpp""

#include <vector>
#include <cstddef>
#include <cstring>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
class ByteFlow {
public:

    /**
     * @brief ByteFlow constructor from pointer and bytes count.
     * @param ptr - pointer to data.
     * @param numBytes - bytes count.
     */
    ByteFlow(const std::byte* ptr, std::size_t numBytes);

    /**
     * @brief takeByteSymbol - method giving one byte symbol.
     * @return byte symbol.
     */
    template <std::size_t num>
    BytesSymbol<num> takeByteSymbol();

    /**
     * @brief finished - is ByteFlow iterated to the end.
     * @return `true if finished iterating.
     */
    template <std::size_t num>
    bool finished() const;

    /**
     * @brief countNumberOfWords.
     * @return number of words.
     */
    template <std::size_t num>
    std::size_t countNumberOfWords() const;

    /**
     * @brief bytesLeft returns number of bytes left.
     * @return number of bytes left.
     */
    std::size_t bytesLeft() const;

private:
    std::vector<std::byte> _bytes;
    std::size_t _currOffset;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
garchiever::ByteFlow::ByteFlow(const std::byte* ptr, std::size_t numBytes)
  : _currOffset(0) {
    _bytes.resize(numBytes);
    std::memcpy(_bytes.data(), ptr, numBytes);
}

//----------------------------------------------------------------------------//
template <std::size_t num>
auto garchiever::ByteFlow::takeByteSymbol() -> BytesSymbol<num> {
    assert(!finished<num>() && "Can`t take bytes.");
    auto ret = BytesSymbol<num>(_bytes.data() + _currOffset);
    _currOffset += num;
    return ret;
}

//----------------------------------------------------------------------------//
template <std::size_t num>
bool garchiever::ByteFlow::finished() const {
    return _currOffset + num >= _bytes.size();
}

//----------------------------------------------------------------------------//
template <std::size_t num>
std::size_t garchiever::ByteFlow::countNumberOfWords() const {
    return _bytes.size() / num;
}

//----------------------------------------------------------------------------//
std::size_t garchiever::ByteFlow::bytesLeft() const {
    return _bytes.size() - _currOffset;
}

#endif // BIT_FLOW_HPP
