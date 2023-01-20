#ifndef BIT_WORD_FLOW_HPP
#define BIT_WORD_FLOW_HPP

#include "word/bits_word.hpp"
#include "data_parser.hpp"

#include <span>
#include <cstdint>
#include <cstddef>
#include <boost/container/static_vector.hpp>

namespace ga::fl {

template <class WordT>
class BitsWordFlow;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWordFlow class
///
template <std::uint16_t _numBits>
class BitsWordFlow<w::BitsWord<_numBits>> {
public:
    using Word = w::BitsWord<_numBits>;
    constexpr static std::uint16_t numBits = Word::numBits;
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
    Iterator begin();

    /**
     * @brief end - get iterator to end.
     * @return itearator word after last word for output.
     */
    Iterator end();

    /**
     * @brief getNumberOfWords - get number of words in data.
     * @return number of words.
     */
    std::size_t getNumberOfWords() const;

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
BitsWordFlow<w::BitsWord<_numBits>>::BitsWordFlow(std::span<const std::byte> data)
    : _data(data) {}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::begin() -> Iterator {
    _data.seek(0);
    return Iterator(_data.getCurrPosBitsIter());
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::end() -> Iterator {
    _data.seek(getNumberOfWords() * _numBits);
    return Iterator(_data.getCurrPosBitsIter());
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
std::size_t BitsWordFlow<w::BitsWord<_numBits>>::getNumberOfWords() const {
    return _data.getNumBytes() * 8 / numBits;
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::getTail() const -> Tail {
     _data.seek(getNumberOfWords() * _numBits);
     return Tail(_data.getCurrPosBitsIter(), _data.getEndBitsIter());
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
class BitsWordFlow<w::BitsWord<_numBits>>::Iterator
        : public boost::iterators::iterator_facade<
            Iterator,
            Word,
            boost::single_pass_traversal_tag,
            Word
        > {
public:
    using type = Iterator;
public:
    //------------------------------------------------------------------------//
    Iterator(DataParser::BitsIterator bitsIteratar)
        : _bitsIterator(bitsIteratar) {}
protected:
    //------------------------------------------------------------------------//
    Word dereference() const;
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
BitsWordFlow<w::BitsWord<_numBits>>::Iterator::dereference() const -> Word {
    return Word(const_cast<DataParser::BitsIterator&>(_bitsIterator));
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
bool
BitsWordFlow<w::BitsWord<_numBits>>::Iterator::equal(
        const Iterator& other) const {
    return _bitsIterator == other._bitsIterator;
}

}


#endif // BIT_WORD_FLOW_HPP
