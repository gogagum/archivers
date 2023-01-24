#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "word/bytes_word.hpp"

#include <span>
#include <cstddef>
#include <cstring>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/container/static_vector.hpp>

namespace ga::fl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes>
class BytesWordFlow {
private:
    using _Word = w::BytesWord<numBytes>;
public:
    constexpr static std::uint16_t numBits = _Word::numBits;
    using Tail = boost::container::static_vector<bool, numBits>;
public:
    class Iterator;
public:

    /**
     * @brief ByteFlow
     * @param bytes
     */
    BytesWordFlow(std::span<const std::byte> bytes);

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
    std::size_t size() const;

    /**
     * @brief getTail
     * @return
     */
    Tail getTail() const;

private:

    std::uint8_t _getTailBytesSize() const;

private:

    std::span<const std::byte> _bytes;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
BytesWordFlow<numBytes>::BytesWordFlow(
        std::span<const std::byte> bytes) : _bytes(bytes) {}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto BytesWordFlow<numBytes>::begin() const -> Iterator {
    return Iterator(_bytes.data());
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto BytesWordFlow<numBytes>::end() const -> Iterator {
    return Iterator(_bytes.data() + size() * numBytes);
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::size_t BytesWordFlow<numBytes>::size() const {
    return _bytes.size() / numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::uint8_t BytesWordFlow<numBytes>::_getTailBytesSize() const {
    return _bytes.size() % numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto BytesWordFlow<numBytes>::getTail() const -> Tail {
    Tail ret;
    auto bytesRng = std::span<const std::byte>(_bytes.end() - _getTailBytesSize(),
                                               _bytes.end());

    for (auto tailByte: bytesRng) {
        ret.insert(ret.end(),
                   ga::impl::bits_begin(tailByte),
                   ga::impl::bits_end(tailByte));
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
class BytesWordFlow<numBytes>::Iterator : public boost::iterator_facade<
    Iterator,
    _Word,
    boost::incrementable_traversal_tag,
    _Word> {
public:
    using type = Iterator;
public:
    //------------------------------------------------------------------------//
    Iterator(const std::byte* ptr) : _ptr(ptr) {};
protected:
    //------------------------------------------------------------------------//
    _Word dereference() const           { return {_ptr}; };
    //------------------------------------------------------------------------//
    bool equal(const type& other) const { return _ptr == other._ptr; };
    //------------------------------------------------------------------------//
    void increment()                    { _ptr += numBytes; };
private:
    const std::byte* _ptr;
private:
    friend class boost::iterators::iterator_core_access;
};

}  // namespace ga

#endif // BIT_FLOW_HPP
