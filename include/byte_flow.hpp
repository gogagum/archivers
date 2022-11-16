#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "bytes_symbol.hpp""
#include "i_sym_flow.hpp"

#include <vector>
#include <cstddef>
#include <cstring>
#include <boost/container/static_vector.hpp>

namespace garchiever {

template <class WordT>
class ByteFlow;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes>
class ByteFlow<BytesSymbol<numBytes>> : ISymFlow<ByteFlow<BytesSymbol<numBytes>>,
                                                 BytesSymbol<numBytes>> {
private:
    using Super = ISymFlow<ByteFlow<BytesSymbol<numBytes>>,
                           BytesSymbol<numBytes>>;
public:
    using This = typename Super::This;
    using Sym = typename Super::Sym;
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
         * @brief operator +=
         * @param offset - number of words to move.
         * @return reference to self after move.
         */
        Iterator& operator+=(std::int64_t offset);

        /**
         * @brief operator --
         * @return reference to self (after decrementing).
         */
        Iterator& operator--();

        /**
         * @brief operator --
         * @return copy of self before decrementing.
         */
        Iterator operator--(int);

        /**
         * @brief operator +
         * @param integral
         * @return
         */
        template<std::integral IntegralT>
        Iterator operator+(IntegralT integral);

        /**
         * @brief operator -=
         * @param offset - number of words to move.
         * @return reference to self after move.
         */
        Iterator& operator-=(std::int64_t offset);

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
        BytesSymbol<numBytes> operator*() const;

    private:
        std::byte* _ptr;
    private:
        friend class ByteFlow;
    };

public:

    /**
     * @brief ByteFlow constructor from pointer and bytes count.
     * @param ptr - pointer to data.
     * @param size - bytes count.
     */
    ByteFlow(const std::byte* ptr, std::size_t size);

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
    std::size_t countNumberOfWords() const;

    /**
     * @brief bytesLeft returns number of bytes left.
     * @return number of bytes left.
     */
    std::size_t bytesLeft() const;

public:  // ISymFlow

    /**
     * @brief getTailSize
     * @return
     */
    virtual std::uint8_t getTailSize() const override;

    /**
     * @brief getTail
     * @return
     */
    virtual boost::container::static_vector<std::byte, Sym::numBytes> getTail() const override {
        std::uint8_t tailSize = getTailSize();
        return boost::container::static_vector<std::byte, Sym::numBytes>(
            _bytes.end() - tailSize - 1, _bytes.end());
    }

private:
    std::vector<std::byte> _bytes;
    std::size_t _currOffset;
};

}  // namespace garchiever

#endif // BIT_FLOW_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::ByteFlow(
        const std::byte* ptr, std::size_t size)
  : _currOffset(0) {
    _bytes.resize(size * numBytes);
    std::memcpy(_bytes.data(), ptr, size);
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::begin(
        ) const -> Iterator
{
    return Iterator(const_cast<std::byte*>(_bytes.data()));
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::end(
        ) const -> Iterator
{
    return Iterator(const_cast<std::byte*>(_bytes.data()) + _bytes.size());
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::size_t
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::countNumberOfWords(
        ) const {
    return _bytes.size() / numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::size_t
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::bytesLeft() const {
    return _bytes.size() - _currOffset;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::uint8_t
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::getTailSize() const {
    return _bytes.size() % numBytes;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::Iterator(
        std::byte* ptr) : _ptr(ptr) {}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::Iterator(
        const Iterator& other) : _ptr(other._ptr) {}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator++(
        ) -> Iterator& {
    _ptr += numBytes;
    return *this;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator++(
        int) -> Iterator {
    Iterator ret(*this);
    _ptr += numBytes;
    return ret;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator+=(
        std::int64_t offset) -> Iterator& {
    _ptr += numBytes * offset;
    return *this;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator--(
        ) -> Iterator& {
    _ptr -= numBytes;
    return *this;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator--(
        int) -> Iterator {
    Iterator ret(*this);
    _ptr -= numBytes;
    return ret;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
template <std::integral IntegralT>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator+(
        IntegralT i) -> Iterator {
    return Iterator(_ptr + i);
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator-=(
        std::int64_t offset) -> Iterator& {
    _ptr -= numBytes * offset;
    return *this;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator==(
        const Iterator& other) const {
    return _ptr == other._ptr;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator!=(
        const Iterator& other) const {
    return _ptr == other._ptr;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto
garchiever::ByteFlow<garchiever::BytesSymbol<numBytes>>::Iterator::operator*(
        ) const -> BytesSymbol<numBytes> {
    return BytesSymbol<numBytes>(_ptr);
}
