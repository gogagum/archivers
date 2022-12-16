#ifndef INTEGER_RANDOM_ACCESS_ITERATOR_HPP
#define INTEGER_RANDOM_ACCESS_ITERATOR_HPP

#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

namespace ga::dict::misc {

template <class T>
class IntegerRandomAccessIterator
        : public boost::iterator_facade<
            IntegerRandomAccessIterator<T>,
            T,
            boost::random_access_traversal_tag,
            T> {
public:
    IntegerRandomAccessIterator(T initial = 0);

    IntegerRandomAccessIterator(const IntegerRandomAccessIterator<T>& other);

    std::ptrdiff_t distance_to(const IntegerRandomAccessIterator& rhs) const;

    void advance(std::ptrdiff_t n);

    T dereference() const;

    bool equal(const IntegerRandomAccessIterator& other) const;

    void increment();

    void decrement();

    T operator[](std::ptrdiff_t offset) const;

  private:
    T _val;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class T>
IntegerRandomAccessIterator<T>::IntegerRandomAccessIterator(T initial)
    : _val(initial) {}

//----------------------------------------------------------------------------//
template <class T>
IntegerRandomAccessIterator<T>::IntegerRandomAccessIterator(
        const IntegerRandomAccessIterator<T>& other)
    : _val(other._val) {}

//----------------------------------------------------------------------------//
template <class T>
std::ptrdiff_t IntegerRandomAccessIterator<T>::distance_to(
        IntegerRandomAccessIterator const& rhs) const {
    return rhs._val - _val;
}

//----------------------------------------------------------------------------//
template <class T>
void IntegerRandomAccessIterator<T>::advance(std::ptrdiff_t n) {
    _val += n;
}

//----------------------------------------------------------------------------//
template <class T>
T IntegerRandomAccessIterator<T>::dereference() const {
    return _val;
}

//----------------------------------------------------------------------------//
template <class T>
bool IntegerRandomAccessIterator<T>::equal(
        const IntegerRandomAccessIterator& other) const {
    return _val == other._val;
};

//----------------------------------------------------------------------------//
template <class T>
void IntegerRandomAccessIterator<T>::increment() {
    ++_val;
}

//----------------------------------------------------------------------------//
template <class T>
void IntegerRandomAccessIterator<T>::decrement() {
    --_val;
}

//----------------------------------------------------------------------------//
template <class T>
T IntegerRandomAccessIterator<T>::operator[](std::ptrdiff_t offset) const {
    return _val + offset;
}

}  // namespace ga::dict::misc

namespace std {

template <class T>
struct iterator_traits<ga::dict::misc::IntegerRandomAccessIterator<T>> {
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using reference = T;
    using difference_type = std::ptrdiff_t;
};


}



#endif // INTEGER_RANDOM_ACCESS_ITERATOR_HPP
