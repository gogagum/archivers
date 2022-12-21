#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "../word/bytes_symbol.hpp"

#include <vector>
#include <cstddef>
#include <cstring>
#include <cstddef>
#include <boost/container/static_vector.hpp>

namespace ga::fl {

template <class WordT>
class ByteFlow;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes>
class ByteFlow<w::BytesSymbol<numBytes>> {
public:
    using Sym = w::BytesSymbol<numBytes>;
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The Iterator class
    ///
    class Iterator {
    private:
        /**
         * @brief Iterator constructor from pointer.
         * @param ptr.
         */
        Iterator(std::byte* ptr);
    public:

        /**
         * @brief Iterator copy constructor.
         * @param other
         */
        Iterator(const Iterator& other);

        /**
         * @brief operator ++
         * @return reference to self (after incrementing).
         */
        Iterator& operator++();

        /**
         * @brief operator ++
         * @return copy of self before incrementing.
         */
        Iterator operator++(int);

        /**
         * @brief operator ==
         * @param other - other iterator to compare.
         * @return `true if iterators are equal.
         */
        bool operator==(const Iterator& other) const;

        /**
         * @brief operator !=
         * @param other - other iterator to compare.
         * @return `true if iterators are not equal.
         */
        bool operator!=(const Iterator& other) const;

        /**
         * @brief operator * get a symbol.
         * @return symbol.
         */
        w::BytesSymbol<numBytes> operator*() const;

    private:
        const std::byte* _ptr;
    private:
        friend class ByteFlow;
    };

public:

    /**
     * @brief ByteFlow
     * @param ptr
     * @param size
     */
    ByteFlow(const void* ptr, std::size_t size);

    /**
     * @brief begin - beginning iterator getter.
     * @return beginning iterator.
     */
    Iterator begin() const;

    /**
     * @brief end - ending iterator getter.
     * @return ending iterator.
     */
    Iterator end() const;

    /**
     * @brief countNumberOfWords.
     * @return number of words.
     */
    std::size_t getNumberOfWords() const;

    /**
     * @brief getTailSize
     * @return
     */
    std::uint8_t getTailSize() const;

    /**
     * @brief getTail
     * @return
     */
    boost::container::static_vector<std::byte, numBytes> getTail() const;

private:
    std::vector<std::byte> _bytes;
    std::size_t _currOffset;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
ByteFlow<w::BytesSymbol<numBytes>>::ByteFlow(const void* ptr, std::size_t size)
        : _currOffset(0) {
    _bytes.resize(size);
    std::memcpy(_bytes.data(), ptr, size);
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto ByteFlow<w::BytesSymbol<numBytes>>::begin() const -> Iterator
{
    return Iterator(const_cast<std::byte*>(_bytes.data()));
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto ByteFlow<w::BytesSymbol<numBytes>>::end() const -> Iterator {
    return Iterator(const_cast<std::byte*>(_bytes.data()
                                           + getNumberOfWords() * numBytes));
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::size_t ByteFlow<w::BytesSymbol<numBytes>>::getNumberOfWords() const {
    return _bytes.size() / numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::uint8_t ByteFlow<w::BytesSymbol<numBytes>>::getTailSize() const {
    return _bytes.size() % numBytes;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
ByteFlow<w::BytesSymbol<numBytes>>::Iterator::Iterator(
        std::byte* ptr) : _ptr(ptr) {}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
ByteFlow<w::BytesSymbol<numBytes>>::Iterator::Iterator(const Iterator& other)
    : _ptr(other._ptr) {}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto ByteFlow<w::BytesSymbol<numBytes>>::Iterator::operator++() -> Iterator& {
    _ptr += numBytes;
    return *this;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto ByteFlow<w::BytesSymbol<numBytes>>::Iterator::operator++(int) -> Iterator {
    Iterator ret(*this);
    _ptr += numBytes;
    return ret;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool ByteFlow<w::BytesSymbol<numBytes>>::Iterator::operator==(
        const Iterator& other) const {
    return _ptr == other._ptr;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool ByteFlow<w::BytesSymbol<numBytes>>::Iterator::operator!=(
        const Iterator& other) const {
    return _ptr != other._ptr;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto ByteFlow<w::BytesSymbol<numBytes>>::Iterator::operator*(
        ) const -> w::BytesSymbol<numBytes> {
    return w::BytesSymbol<numBytes>(_ptr);
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
boost::container::static_vector<std::byte, numBytes>
ByteFlow<w::BytesSymbol<numBytes>>::getTail() const {
    std::uint8_t tailSize = getTailSize();
    return boost::container::static_vector<std::byte, numBytes>(
        _bytes.end() - tailSize, _bytes.end());
}

}  // namespace ga

#endif // BIT_FLOW_HPP
