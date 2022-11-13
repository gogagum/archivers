#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "bytes_symbol.hpp""
#include "i_words_flow.hpp"

#include <vector>
#include <cstddef>
#include <cstring>

namespace garchiever {

template <class WordT>
class ByteFlow;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::size_t numBytes>
class ByteFlow<BytesSymbol<numBytes>> : IWordsFlow<ByteFlow<BytesSymbol<numBytes>>,
                                                   BytesSymbol<numBytes>> {
private:
    using Super = IWordsFlow<ByteFlow<BytesSymbol<numBytes>>,
                             BytesSymbol<numBytes>>;
public:
    using This = typename Super::This;
    using Word = typename Super::Word;

public:

    /**
     * @brief ByteFlow constructor from pointer and bytes count.
     * @param ptr - pointer to data.
     * @param size - bytes count.
     */
    ByteFlow(const std::byte* ptr, std::size_t size);

    /**
     * @brief takeByteSymbol - method giving one byte symbol.
     * @return byte symbol.
     */
    Word takeByteSymbol();

    /**
     * @brief finished - is ByteFlow iterated to the end.
     * @return `true if finished iterating.
     */
    bool finished() const;

    /**
     * @brief countNumberOfWords.
     * @return number of words.
     */
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
template <std::size_t numBytes>
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::ByteFlow(
        const std::byte* ptr, std::size_t size)
  : _currOffset(0) {
    _bytes.resize(size);
    std::memcpy(_bytes.data(), ptr, size);
}

//----------------------------------------------------------------------------//
template <std::size_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::takeByteSymbol(
        ) -> Word {
    assert(!finished() && "Can`t take bytes.");
    auto ret = BytesSymbol<numBytes>(_bytes.data() + _currOffset);
    _currOffset += numBytes;
    return ret;
}

//----------------------------------------------------------------------------//
template <std::size_t numBytes>
bool garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::finished() const {
    return _currOffset + numBytes >= _bytes.size();
}

//----------------------------------------------------------------------------//
template <std::size_t numBytes>
std::size_t
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::countNumberOfWords(
        ) const {
    return _bytes.size() / numBytes;
}

//----------------------------------------------------------------------------//
template <std::size_t numBytes>
std::size_t
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::bytesLeft() const {
    return _bytes.size() - _currOffset;
}

#endif // BIT_FLOW_HPP
