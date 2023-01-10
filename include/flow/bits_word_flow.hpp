#ifndef BIT_WORD_FLOW_HPP
#define BIT_WORD_FLOW_HPP

#include "../word/bits_word.hpp"
#include "../data_parser.hpp"

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
    using Sym = w::BitsWord<_numBits>;
    constexpr static std::uint16_t numBits = Sym::numBits;
    using Tail = boost::container::static_vector<bool, numBits>;
public:
    class Iterator;
public:

    BitsWordFlow(std::vector<std::byte>&& data);

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
    DataParser _data;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
BitsWordFlow<w::BitsWord<_numBits>>::BitsWordFlow(
        std::vector<std::byte>&& data) : _data(data) {}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::begin() -> Iterator {
    return Iterator(_data);
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::end() -> Iterator {
    return Iterator(_data, getNumberOfWords() * _numBits);
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
std::size_t BitsWordFlow<w::BitsWord<_numBits>>::getNumberOfWords() const {
    return _data.getNumBytes() * 8 / numBits;
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::getTail() const -> Tail {

}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
class BitsWordFlow<w::BitsWord<_numBits>>::Iterator
        : public boost::iterators::iterator_facade<
            Iterator,
            Sym,
            boost::incrementable_traversal_tag,
            Sym
        > {
public:
    using type = Iterator;
public:
    //------------------------------------------------------------------------//
    Iterator(DataParser& dataParser, std::size_t offsetFromStart = 0)
        : _dataParser(dataParser), _offsetFromStart(offsetFromStart) {}
protected:
    //------------------------------------------------------------------------//
    Sym dereference() const;
    //------------------------------------------------------------------------//
    bool equal(const type& other) const;
    //------------------------------------------------------------------------//
    void increment()                    { _offsetFromStart += numBits; }
private:
    DataParser& _dataParser;
    std::size_t _offsetFromStart;
private:
    friend class boost::iterators::iterator_core_access;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWordFlow<w::BitsWord<_numBits>>::Iterator::dereference() const -> Sym {
    _dataParser.seek(_offsetFromStart);
    boost::container::static_vector<bool, numBits> bits;
    for (auto _: boost::irange<std::uint16_t>(0, _numBits)) {
        bits.push_back(_dataParser.takeBit());
    }
    return Sym(bits.begin());
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
bool
BitsWordFlow<w::BitsWord<_numBits>>::Iterator::equal(
        const Iterator& other) const {
    return _dataParser == other._dataParser
            && _offsetFromStart == other._offsetFromStart;
}

}


#endif // BIT_WORD_FLOW_HPP
