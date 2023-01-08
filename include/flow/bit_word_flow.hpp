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

    Iterator begin();

    Iterator end();

    std::size_t getNumberOfWords() const;

    Tail getTail() const;

private:
    DataParser _data;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
BitsWordFlow<w::BitsWord<_numBits>>::BitsWordFlow(
        std::vector<std::byte>&& data) : _data(std::move(data)) {}


}

#endif // BIT_WORD_FLOW_HPP
