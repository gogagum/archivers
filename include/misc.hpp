#pragma once

#ifndef MISC_HPP
#define MISC_HPP

#include <array>
#include <ostream>
#include <bitset>
#include <cstddef>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace ga::w {
//----------------------------------------------------------------------------//
std::ostream& operator<< (std::ostream& os, std::byte b);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
std::ostream& operator<< (std::ostream& os, std::byte b) {
   return os << std::bitset<8>(std::to_integer<int>(b));
}

}  // namespace ga

namespace ga::impl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsIterator class
///
template <class T>
class BitsIterator : public boost::iterators::iterator_facade<
        BitsIterator<T>,
        bool,
        boost::random_access_traversal_tag,
        bool> {
private:
    using type = BitsIterator<T>;
public:
    //------------------------------------------------------------------------//
    BitsIterator(const T& iterated)
        : _iterated(iterated), _offset(sizeof(T) * 8) {}
    //------------------------------------------------------------------------//
    BitsIterator(const T& iterated, std::ptrdiff_t offset)
        : _iterated(iterated), _offset(offset) {}
    //------------------------------------------------------------------------//
    BitsIterator& operator=(const BitsIterator& other);
protected:
    //------------------------------------------------------------------------//
    std::ptrdiff_t
    distance_to(const type& rhs) const  { return rhs._offset - _offset; }
    //------------------------------------------------------------------------//
    void advance(std::ptrdiff_t n)      { _offset += n; }
    //------------------------------------------------------------------------//
    bool dereference() const;
    //------------------------------------------------------------------------//
    bool equal(const type& other) const;
    //------------------------------------------------------------------------//
    void increment()                    { ++_offset; }
    //------------------------------------------------------------------------//
    void decrement()                    { --_offset; }

private:
    const T& _iterated;
    std::ptrdiff_t _offset;
private:
    friend class boost::iterators::iterator_core_access;
};

//----------------------------------------------------------------------------//
template <class T>
BitsIterator<T> bits_begin(const T& t) {
    return BitsIterator(t, 0);
}

//----------------------------------------------------------------------------//
template <class T>
BitsIterator<T> bits_end(const T& t) {
    return BitsIterator(t);
}

//----------------------------------------------------------------------------//
template <class T>
auto make_bits_iterator_range(const T& t) {
    return boost::make_iterator_range(bits_begin(t), bits_end(t));
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class T>
BitsIterator<T>& BitsIterator<T>::operator=(const BitsIterator& other) {
    const_cast<T&>(_iterated) = other._iterated;
    _offset = other._offset;
    return *this;
}

//----------------------------------------------------------------------------//
template <class T>
bool BitsIterator<T>::dereference() const {
    using ConstBytesArray = const std::array<std::byte, sizeof(T)>;
    const auto& iteratedBytes =
            reinterpret_cast<ConstBytesArray&>(_iterated);
    const auto bytesOffset = sizeof(T) - _offset / 8 - 1;
    const std::byte b = iteratedBytes[bytesOffset];
    const auto inByteOffset = 8 - (_offset % 8) - 1;
    return (((b >> inByteOffset)) & std::byte{1}) == std::byte{1};
}

//----------------------------------------------------------------------------//
template <class T>
bool BitsIterator<T>::equal(const type& other) const {
    return &_iterated == &other._iterated
        && _offset == other._offset;
}


}  // namespace ga::impl

#endif // MISC_HPP


