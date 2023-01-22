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

template <class WordT>
class BytesWordFlow;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes>
class BytesWordFlow<w::BytesWord<numBytes>> {
public:
    using Word = w::BytesWord<numBytes>;
    constexpr static std::uint16_t numBits = Word::numBits;
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
    std::size_t getWordsCount() const;

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
BytesWordFlow<w::BytesWord<numBytes>>::BytesWordFlow(
        std::span<const std::byte> bytes) : _bytes(bytes) {}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto BytesWordFlow<w::BytesWord<numBytes>>::begin() const -> Iterator {
    return Iterator(_bytes.data());
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto BytesWordFlow<w::BytesWord<numBytes>>::end() const -> Iterator {
    return Iterator(_bytes.data() + getWordsCount() * numBytes);
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::size_t BytesWordFlow<w::BytesWord<numBytes>>::getWordsCount() const {
    return _bytes.size() / numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::uint8_t BytesWordFlow<w::BytesWord<numBytes>>::_getTailBytesSize() const {
    return _bytes.size() % numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto BytesWordFlow<w::BytesWord<numBytes>>::getTail() const ->Tail {
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
class BytesWordFlow<w::BytesWord<numBytes>>::Iterator
        : public boost::iterator_facade<
            Iterator,
            Word,
            boost::incrementable_traversal_tag,
            Word> {
public:
    using type = Iterator;
public:
    //------------------------------------------------------------------------//
    Iterator(const std::byte* ptr) : _ptr(ptr) {};
protected:
    //------------------------------------------------------------------------//
    Word dereference() const             { return {_ptr}; };
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
