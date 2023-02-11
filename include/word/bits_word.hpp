#ifndef BITS_SYMBOL_HPP
#define BITS_SYMBOL_HPP

#include <array>
#include <algorithm>
#include <numeric>
#include <boost/range/irange.hpp>
#include <cstddef>
#include <ostream>
#include <concepts>
#include <cstdint>

#include "bits_iterator.hpp"
#include "tags.hpp"

namespace ga::w{

namespace rng = std::ranges;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWord class
///
template <std::uint16_t _numBits>
class BitsWord {
public:

    using OutType = tags::BitsOut;
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
    static BitsWord<_numBits>
    byOrd(std::uint64_t ord) { return BitsWord<_numBits>(ord); }

public:

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
    void bitsOut(IterT outIter) const;

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
template <std::input_iterator IterT>
BitsWord<_numBits>::BitsWord(IterT inputIter)
    : _data{std::accumulate(
                inputIter, inputIter + _numBits, 0ull,
                [](auto curr, bool bit) { return (curr << 1) | bit; })} {}

////////////////////////////////////////////////////////////////////////////////
template <std::uint16_t _numBits>
template <std::output_iterator<bool> IterT>
void BitsWord<_numBits>::bitsOut(IterT outIter) const {
    std::copy(ga::impl::bits_end(_data) - _numBits,
              ga::impl::bits_end(_data),
              outIter);
}

////////////////////////////////////////////////////////////////////////////////
template <std::uint16_t numBits>
std::ostream& operator<<(std::ostream& os, BitsWord<numBits> bw) {
    return *std::accumulate(
        ga::impl::bits_begin(bw._data), ga::impl::bits_end(bw._data), &os,
        [](auto* os, const bool bit) { return &(*os << bit); });
}

}

#endif // BITS_SYMBOL_HPP
