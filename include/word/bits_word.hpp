#ifndef BITS_SYMBOL_HPP
#define BITS_SYMBOL_HPP

#include <array>
#include <algorithm>
#include <boost/range/irange.hpp>
#include <cstddef>
#include <ostream>
#include <concepts>
#include <cstdint>

namespace ga::w{

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWord class
///
template <std::uint16_t _numBits>
class BitsWord {
public:

    constexpr static std::uint16_t numBits = _numBits;
    constexpr static std::uint64_t wordsCount = 1ull << numBits;

public:

    /**
     * @brief ord - get word index in order of words.
     * @param word - to get order of.
     * @return word order index.
     */
    static std::uint64_t ord(const BitsWord<_numBits>& word);

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
    explicit BitsWord(IterT& inputIter);

    /**
     * @brief BitsWord - constructor from bits input iterator.
     * @param inputIter - input iterator to take bits form.
     */
    template <std::input_iterator IterT>
    explicit BitsWord(IterT&& inputIter);

    /**
     * @brief bitsOut - give bits of a word out.
     * @param outIter - outPut bits iterator to erite bits.
     */
    template <std::output_iterator<bool> IterT>
    void bitsOut(IterT outIter) const;

private:
    std::array<bool, numBits> _bits;

private:

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    template <std::uint16_t __numBits>
    friend bool operator==(const BitsWord<__numBits>& bs1,
                           const BitsWord<__numBits>& bs2);

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    template <std::uint16_t __numBits>
    friend bool operator!=(const BitsWord<__numBits>& bs1,
                           const BitsWord<__numBits>& bs2);

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
/// \brief operator ==
/// \param bw1 - first compared bit symbol.
/// \param bw2 - second compared bit symbol.
/// \return `true if symbols are equal, else `false.
///
template <std::uint16_t numBits>
bool operator==(const BitsWord<numBits>& bw1, const BitsWord<numBits>& bw2);

////////////////////////////////////////////////////////////////////////////////
/// \brief operator ==
/// \param bw1 - first compared bit symbol.
/// \param bw2 - second compared bit symbol.
/// \return `true if symbols are not equal, else `false.
///
template <std::uint16_t numBits>
bool operator!=(const BitsWord<numBits>& bw1, const BitsWord<numBits>& bw2);

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
std::uint64_t BitsWord<_numBits>::ord(const BitsWord<_numBits>& bw) {
    auto ret = std::uint64_t{0};
    for (auto bit : bw._bits) {
        ret <<= 1;
        ret |= bit ? std::uint64_t{1} : std::uint64_t{0};
    }
    return ret;
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
BitsWord<_numBits> BitsWord<_numBits>::byOrd(std::uint64_t ord) {
    BitsWord<_numBits> ret;
    for (auto i : boost::irange<std::uint16_t>(0, _numBits)) {
        ret._bits[i] = (ord >> (numBits - i - 1)) & 1;
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
template <std::input_iterator IterT>
BitsWord<_numBits>::BitsWord(IterT& iter) {
    for (auto& bit : _bits) {
        bit = *iter;
        ++iter;
    }
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
template <std::input_iterator IterT>
BitsWord<_numBits>::BitsWord(IterT&& iter) {
    for (auto& bit : _bits) {
        bit = *iter;
        ++iter;
    }
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
template <std::output_iterator<bool> IterT>
void BitsWord<_numBits>::bitsOut(IterT outIter) const {
    std::ranges::copy(_bits, outIter);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
bool operator==(const BitsWord<_numBits>& bw1, const BitsWord<_numBits>& bw2) {
    return bw1._bits == bw2._bits;
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
bool operator!=(const BitsWord<_numBits>& bw1, const BitsWord<_numBits>& bw2) {
    return bw1._bits != bw2._bits;
}

//----------------------------------------------------------------------------//
template <std::uint16_t numBits>
std::ostream& operator<<(std::ostream& os, BitsWord<numBits> bw) {
    for (auto bit: bw._bits) {
        os << bit;
    }
    return os;
}

}

#endif // BITS_SYMBOL_HPP
