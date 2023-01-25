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
    static Ord ord(const BitsWord<_numBits>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static BitsWord<_numBits> byOrd(std::uint64_t ord);

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
    BitsWord(IterT inputIter) { std::copy_n(inputIter, _numBits, _bits.begin()); }

    /**
     * @brief bitsOut - give bits of a word out.
     * @param outIter - outPut bits iterator to erite bits.
     */
    template <std::output_iterator<bool> IterT>
    void bitsOut(IterT outIter) const { std::ranges::copy(_bits, outIter); }

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

    std::array<bool, numBits> _bits;

private:

    /**
     * @brief operator <<  for debug output.
     * @param os - output stream.
     * @param word - printed word.
     * @return output stream reference.
     * @return
     */
    template <std::uint16_t __numBits>
    friend std::ostream& operator<<(std::ostream& os, BitsWord<__numBits> word);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief operator <<
/// \param os - stream to write to.
/// \param bw - ounput word.
/// \return reference to os.
///
template <std::uint16_t numBits>
std::ostream& operator<<(std::ostream& os, BitsWord<numBits> bw);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
auto BitsWord<_numBits>::ord(const BitsWord<_numBits>& bw) -> Ord {
    return std::accumulate(
        bw._bits.begin(), bw._bits.end(), std::uint64_t{0},
        [](auto curr, bool bit) { return (curr << 1) | std::uint64_t(bit); });
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
BitsWord<_numBits> BitsWord<_numBits>::byOrd(std::uint64_t ord) {
    return BitsWord<_numBits>(ga::impl::bits_end(ord) - _numBits);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t numBits>
std::ostream& operator<<(std::ostream& os, BitsWord<numBits> bw) {
    std::ranges::for_each(bw._bits, [&](auto bit) { os << (bit ? 1 : 0); });
    return os;
}

}

#endif // BITS_SYMBOL_HPP
