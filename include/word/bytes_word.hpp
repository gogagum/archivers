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
#include <boost/multiprecision/cpp_int.hpp>

#include "bits_iterator.hpp"

namespace ga::w {

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
    static std::uint64_t ord(const BytesWord<_numBytes>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static BytesWord<_numBytes> byOrd(std::uint64_t ord);

public:

    /**
     * Default constructor. Constructs a symbol filled with zeros,
     */
    BytesWord() = default;

    /**
     * @brief BytesSymbol bytes symbol constructor from pointer to data.
     * @param ptr - pointer to data.
     */
    BytesWord(const std::byte* ptr);

    /**
     * @brief BytesSymbol constructor from array.
     * @param bytes
     */
    BytesWord(const std::array<std::byte, _numBytes>& bytes);

    template <std::output_iterator<std::byte> IterT>
    void bytesOut(IterT outIter) const;

private:

    std::uint64_t _data;  // data of the word

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
/// \brief operator <<
/// \param os - stream to write to.
/// \param sym - ounput symbol.
/// \return reference to os.
///
template <std::uint8_t numBytes>
std::ostream& operator<<(std::ostream& os, BytesWord<numBytes> sym);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes>
std::uint64_t
BytesWord<_numBytes>::ord(const BytesWord<_numBytes>& word) {
    return word._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes>
auto BytesWord<_numBytes>::byOrd(std::uint64_t ord) -> BytesWord<_numBytes> {
    assert(ord < wordsCount && "Too big ord.");
    auto ret = BytesWord<_numBytes>();
    ret._data = ord;
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
BytesWord<numBytes>::BytesWord(const std::byte* ptr)
    : _data{std::accumulate(
                ptr, ptr + numBytes, 0ull,
                [](auto curr, std::byte b) {
                    return (curr << 8) | std::to_integer<std::uint64_t>(b);
                })} {}

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes>
BytesWord<_numBytes>::BytesWord(const std::array<std::byte, _numBytes>& arr)
    : BytesWord(arr.data()) {};

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes>
template <std::output_iterator<std::byte> IterT>
void BytesWord<_numBytes>::bytesOut(IterT outIter) const {
    auto& bytes = reinterpret_cast<const std::array<std::byte, 8>&>(_data);
    std::copy(bytes.rend() - _numBytes, bytes.rend(), outIter);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::ostream& operator<<(std::ostream& os, BytesWord<numBytes> word) {
    return os << word._data;  // TODO: in a binary form.
}

}  // namespace ga::w

#endif // BYTES_SYMBOL_HPP
