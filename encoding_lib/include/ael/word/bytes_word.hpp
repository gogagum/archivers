#ifndef BYTES_SYMBOL_HPP
#define BYTES_SYMBOL_HPP

#include <array>
#include <bitset>
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <cassert>
#include <cstring>
#include <ostream>
#include <boost/range/irange.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <ael/bits_iterator.hpp>
#include <ael/bytes_iterator.hpp>

namespace ael::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesSymbol class
///
template <std::uint8_t _numBytes>
class BytesWord {
public:

    constexpr static std::uint16_t numBits = _numBytes * 8;
    using Ord = std::uint64_t;
    constexpr static Ord wordsCount = Ord{1} << numBits;

public:

    /**
     * @brief ord - 
     * @param word
     * @return
     */
    static std::uint64_t
    ord(const BytesWord<_numBytes>& word) { return word._data; }

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static BytesWord<_numBytes> byOrd(std::uint64_t ord);

private:

    BytesWord(std::uint64_t ord) : _data(ord) {}

public:

    /**
     * Default constructor. Constructs a symbol filled with zeros,
     */
    BytesWord() = default;

    /**
     * @brief BytesSymbol bytes symbol constructor from pointer to data.
     * @param ptr - pointer to data.
     */
    template <std::input_iterator IterT>
    explicit BytesWord(IterT iter);

    /**
     * @brief bytesOut - put out word bytes.
     * @param outIter iterator to put to.
     */
    template <std::output_iterator<std::byte> IterT>
    void
    bytesOut(IterT outIter) const { boost::range::copy(_getBytes(), outIter); }

private:

    std::uint64_t _data;  // data of the word

private:

    using BitsIter = impl::BitsIterator<std::uint64_t>;
    using BytesIter = impl::BytesIterator<std::uint64_t>;

private:

    boost::iterator_range<BitsIter> _getBits() const
    { return { BitsIter(_data) - 8 * _numBytes, BitsIter(_data) }; }

    boost::iterator_range<BytesIter> _getBytes() const
    { return { BytesIter(_data) - _numBytes, BytesIter(_data) }; }

private:


    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    friend bool operator==(const BytesWord& bs1, const BytesWord& bs2) = default;

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    friend bool operator!=(const BytesWord& bs1, const BytesWord& bs2) = default;

    /**
     * @brief operator << for debug output.
     * @param os - output stream.
     * @param word - printed word.
     * @return output stream reference.
     */
    template <std::uint8_t __numBytes>
    friend std::ostream& operator<<(std::ostream& os,
                                    BytesWord<__numBytes> word);
};

////////////////////////////////////////////////////////////////////////////////
template <std::uint8_t _numBytes>
BytesWord<_numBytes> BytesWord<_numBytes>::byOrd(std::uint64_t ord) {
    assert(ord < wordsCount && "Too big ord.");
    return BytesWord<_numBytes>(ord);
}

////////////////////////////////////////////////////////////////////////////////
template <std::uint8_t numBytes>
template <std::input_iterator IterT>
BytesWord<numBytes>::BytesWord(IterT iter)
    : _data{std::accumulate(
                iter, iter + numBytes, 0ull,
                [](auto curr, std::byte b) {
                    return (curr << 8) | std::to_integer<std::uint64_t>(b);
                })} {}

////////////////////////////////////////////////////////////////////////////////
template <std::uint8_t numBytes>
std::ostream& operator<<(std::ostream& os, BytesWord<numBytes> word) {
    return *boost::accumulate(word._getBits(), &os,
        [](auto* os, const bool bit) { return &(*os << bit); });
}

}  // namespace ael::w

#endif // BYTES_SYMBOL_HPP
