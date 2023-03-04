#ifndef BIT_WORD_FLOW_HPP
#define BIT_WORD_FLOW_HPP

#include <ael/word/bits_word.hpp>
#include <ael/data_parser.hpp>

#include <span>
#include <cstdint>
#include <cstddef>
#include <boost/container/static_vector.hpp>

namespace ael::fl {

namespace bi = boost::iterators;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWordFlow class
///
template <std::uint16_t _numBits>
class BitsWordFlow {
private:
    using _Word = ael::w::BitsWord<_numBits>;
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
    BitsWordFlow(std::span<const std::byte> data) : _data(data) {}

    /**
     * @brief begin - get iterator to begin.
     * @return iterator to first word for output.
     */
    Iterator begin() const { return Iterator(_data.getBeginBitsIter()); }

    /**
     * @brief end - get iterator to end.
     * @return itearator word after last word for output.
     */
    Iterator end() const { return Iterator(_data.getBeginBitsIter()) + size(); }

    /**
     * @brief getNumberOfWords - get number of words in data.
     * @return number of words.
     */
    std::size_t size() const { return _data.getNumBytes() * 8 / numBits; }

    /**
     * @brief getTail get tail bits.
     * @return tail bits array.
     */
    Tail getTail() const
    { return Tail(_data.getBeginBitsIter() + size() * _numBits, _data.getEndBitsIter()); }

private:
    mutable DataParser _data;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWordFlow::Iterator class
///
template <std::uint16_t _numBits>
class BitsWordFlow<_numBits>::Iterator : public bi::iterator_facade<
    Iterator,
    _Word,
    boost::random_access_traversal_tag,
    _Word
> {
public:
    using type = Iterator;
private:
    using BitsIterator = DataParser::BitsIterator;
public:
    Iterator(BitsIterator bitsIterator) : _bitsIter(bitsIterator) {}
protected:
    ////////////////////////////////////////////////////////////////////////////
    _Word dereference() const           { return _Word(_bitsIter); }
    ////////////////////////////////////////////////////////////////////////////
    std::ptrdiff_t
    distance_to (const type& other)     { return other._bitsIter - _bitsIter; }
    ////////////////////////////////////////////////////////////////////////////
    bool equal(const type& other) const { return _bitsIter == other._bitsIter; }
    ////////////////////////////////////////////////////////////////////////////
    void increment()                    { _bitsIter += _numBits; }
    ////////////////////////////////////////////////////////////////////////////
    void advance(std::ptrdiff_t offset) { _bitsIter += offset * _numBits; }
private:
    BitsIterator _bitsIter;
private:
    friend class boost::iterators::iterator_core_access;
};

}  // namespace ael::fl

#endif // BIT_WORD_FLOW_HPP
