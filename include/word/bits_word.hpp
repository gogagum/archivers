#ifndef BITS_SYMBOL_HPP
#define BITS_SYMBOL_HPP

#include <array>
#include <algorithm>
#include <numeric>
#include <boost/range/irange.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <cstddef>
#include <ostream>
#include <concepts>
#include <cstdint>

#include "bits_iterator.hpp"

namespace ga::w{

namespace rng = std::ranges;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWord class
///
template <std::uint16_t _numBits>
class BitsWord {
public:

    using Ord = std::uint64_t;

public:

    constexpr static std::uint16_t numBits = _numBits;
    constexpr static std::uint64_t wordsCount = 1ull << numBits;

public:

    /**
     * @brief ord - get word index in order of words.
     * @param word - to get order of.
     * @return word order index.
     */
    static Ord ord(const BitsWord<_numBits>& word) { return word._data; }

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static BitsWord<_numBits> byOrd(std::uint64_t ord);

private:

    BitsWord(std::uint64_t ord) : _data(ord) {}

public:

    /**
     * Default constructor. Constructs a symbol filled with zeros,
     */
    BitsWord() = default;

    /**
     * @brief BitsWord - constructor from bits input iterator.
     * @param inputIter - input iterator to take bits form.
     */
    template <std::input_iterator IterT>
    explicit BitsWord(IterT inputIter);

    /**
     * @brief bitsOut - give bits of a word out.
     * @param outIter - outPut bits iterator to erite bits.
     */
    template <std::output_iterator<bool> IterT>
    void
    bitsOut(IterT outIter) const { boost::range::copy(_getBits(), outIter); }

public:

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    friend bool operator==(const BitsWord& bs1, const BitsWord& bs2) = default;

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    friend bool operator!=(const BitsWord& bs1, const BitsWord& bs2) = default;

private:

    std::uint64_t _data;

private:

    using BitsIter = ga::impl::BitsIterator<std::uint64_t>;

private:

    boost::iterator_range<BitsIter>
    _getBits() const { return { BitsIter(_data) - _numBits, BitsIter(_data) }; }

private:

    /**
     * @brief operator << for debug output.
     * @param os - output stream.
     * @param word - printed word.
     * @return output stream reference.
     * @return
     */
    template <std::uint16_t __numBits>
    friend std::ostream& operator<<(std::ostream& os, BitsWord<__numBits> word);
};

////////////////////////////////////////////////////////////////////////////////
template <std::uint16_t _numBits>
BitsWord<_numBits> BitsWord<_numBits>::byOrd(std::uint64_t ord) {
    assert(ord < wordsCount && "Too big ord.");
    return BitsWord<_numBits>(ord);
}

////////////////////////////////////////////////////////////////////////////////
template <std::uint16_t _numBits>
template <std::input_iterator IterT>
BitsWord<_numBits>::BitsWord(IterT inputIter)
    : _data{std::accumulate(
                inputIter, inputIter + _numBits, 0ull,
                [](auto curr, bool bit) { return (curr << 1) | bit; })} {}

////////////////////////////////////////////////////////////////////////////////
template <std::uint16_t numBits>
std::ostream& operator<<(std::ostream& os, BitsWord<numBits> bw) {
    return *boost::accumulate(bw._getBits(), &os,
                [](auto* os, bool bit) { return &(*os << bit); });
}

}

#endif // BITS_SYMBOL_HPP
