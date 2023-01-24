#ifndef BIT_WORD_FLOW_HPP
#define BIT_WORD_FLOW_HPP

#include "word/bits_word.hpp"
#include "data_parser.hpp"

#include <span>
#include <cstdint>
#include <cstddef>
#include <boost/container/static_vector.hpp>

namespace ga::fl {

namespace bi = boost::iterators;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWordFlow class
///
template <std::uint16_t _numBits>
class BitsWordFlow {
private:
    using _Word = ga::w::BitsWord<_numBits>;
public:
    constexpr static std::uint16_t numBits = w::BitsWord<_numBits>::numBits;
    using Tail = boost::container::static_vector<bool, numBits>;
private:
    class Iterator;
public:

    /**
     * @brief BitsWordFlow constructor from span.
     * @param data - bytes to create words.
     */
    BitsWordFlow(std::span<const std::byte> data);

    /**
     * @brief begin - get iterator to begin.
     * @return iterator to first word for output.
     */
    Iterator begin() const;

    /**
     * @brief end - get iterator to end.
     * @return itearator word after last word for output.
     */
    Iterator end() const;

    /**
     * @brief getNumberOfWords - get number of words in data.
     * @return number of words.
     */
    std::size_t size() const;

    /**
     * @brief getTail get tail bits.
     * @return tail bits array.
     */
    Tail getTail() const;

private:
    mutable DataParser _data;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
BitsWordFlow<_numBits>::BitsWordFlow(std::span<const std::byte> data)
    : _data(data) {}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<_numBits>::begin() const -> Iterator {
    _data.seek(0);
    return Iterator(_data.getCurrPosBitsIter());
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<_numBits>::end() const -> Iterator {
    _data.seek(size() * _numBits);
    return Iterator(_data.getCurrPosBitsIter());
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
std::size_t BitsWordFlow<_numBits>::size() const {
    return _data.getNumBytes() * 8 / numBits;
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<_numBits>::getTail() const -> Tail {
     _data.seek(size() * _numBits);
     return Tail(_data.getCurrPosBitsIter(), _data.getEndBitsIter());
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
class BitsWordFlow<_numBits>::Iterator : public bi::iterator_facade<
    Iterator,
    _Word,
    boost::single_pass_traversal_tag,
    _Word
> {
public:
    using type = Iterator;
public:
    //------------------------------------------------------------------------//
    Iterator(DataParser::BitsIterator bitsIteratar)
        : _bitsIterator(bitsIteratar) {}
protected:
    //------------------------------------------------------------------------//
    _Word dereference() const;
    //------------------------------------------------------------------------//
    bool equal(const type& other) const;
    //------------------------------------------------------------------------//
    void increment()                    { }
private:
    DataParser::BitsIterator _bitsIterator;
private:
    friend class boost::iterators::iterator_core_access;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto
BitsWordFlow<_numBits>::Iterator::dereference() const -> _Word {
    return w::BitsWord<_numBits>(
                const_cast<DataParser::BitsIterator&>(_bitsIterator));
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
bool
BitsWordFlow<_numBits>::Iterator::equal(
        const Iterator& other) const {
    return _bitsIterator == other._bitsIterator;
}

}


#endif // BIT_WORD_FLOW_HPP
