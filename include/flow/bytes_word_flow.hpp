#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "../word/bytes_symbol.hpp"

#include <vector>
#include <cstddef>
#include <cstring>
#include <cstddef>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/container/static_vector.hpp>

namespace ga::fl {

template <class WordT>
class BytesWordFlow;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes>
class BytesWordFlow<w::BytesSymbol<numBytes>> {
public:
    using Sym = w::BytesSymbol<numBytes>;
    constexpr static std::uint16_t numBits = Sym::numBits;
    using Tail = boost::container::static_vector<bool, numBits>;
public:
    class Iterator;
public:

    /**
     * @brief ByteFlow
     * @param ptr
     * @param size
     */
    BytesWordFlow(const void* ptr, std::size_t size);

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
     * @brief getTail
     * @return
     */
    Tail getTail() const;

private:

    std::uint8_t _getTailBytesSize() const;

private:

    std::vector<std::byte> _bytes;
    std::size_t _currOffset;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
BytesWordFlow<w::BytesSymbol<numBytes>>::BytesWordFlow(const void* ptr, std::size_t size)
        : _currOffset(0) {
    _bytes.resize(size);
    std::memcpy(_bytes.data(), ptr, size);
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto BytesWordFlow<w::BytesSymbol<numBytes>>::begin() const -> Iterator {
    return Iterator(_bytes.data());
}

//----------------------------------------------------------------------------//
template<std::uint8_t numBytes>
auto BytesWordFlow<w::BytesSymbol<numBytes>>::end() const -> Iterator {
    return Iterator(_bytes.data() + getNumberOfWords() * numBytes);
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::size_t BytesWordFlow<w::BytesSymbol<numBytes>>::getNumberOfWords() const {
    return _bytes.size() / numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::uint8_t BytesWordFlow<w::BytesSymbol<numBytes>>::_getTailBytesSize() const {
    return _bytes.size() % numBytes;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
auto BytesWordFlow<w::BytesSymbol<numBytes>>::getTail() const ->Tail {
    Tail ret;
    auto bytesRng =
            boost::make_iterator_range(_bytes.end() - _getTailBytesSize(),
                                       _bytes.end());
    for (auto tailByte: bytesRng) {
        ret.insert(ret.end(),
                   ga::impl::bits_begin(tailByte),
                   ga::impl::bits_end(tailByte));
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
template <std::uint8_t numBytes>
class BytesWordFlow<w::BytesSymbol<numBytes>>::Iterator
        : public boost::iterator_facade<
            Iterator,
            Sym,
            boost::incrementable_traversal_tag,
            Sym> {
public:
    using type = Iterator;
public:
    //------------------------------------------------------------------------//
    Iterator(const std::byte* ptr) : _ptr(ptr) {};
protected:
    //------------------------------------------------------------------------//
    Sym dereference() const             { return {_ptr}; };
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
