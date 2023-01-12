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
#include <boost/range/combine.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "ord_t_choose.hpp"
#include "../bits_iterator.hpp"

namespace ga::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesSymbol class
///
template <std::uint8_t _numBytes,
          class CountT = std::uint64_t>
class BytesWord {
public:

    constexpr static std::uint16_t numBits = _numBytes * 8;
    using Ord = typename impl::OrdTChoose<numBits>::Type;
    constexpr static Ord wordsCount = Ord{1} << numBits;
    using Count = CountT;

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

    /**
     * @brief toBits transform to bits
     * @return bits array
     */
    template <std::output_iterator<bool> IterT>
    void bitsOut(IterT outIter) const;

private:
    std::array<std::byte, _numBytes> _data;  // data of the word

private:

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    template <std::uint8_t __numBytes>
    friend bool operator==(const BytesWord<__numBytes>& bs1,
                           const BytesWord<__numBytes>& bs2);

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    template <std::uint8_t __numBytes>
    friend bool operator!=(const BytesWord<__numBytes>& bs1,
                           const BytesWord<__numBytes>& bs2);

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
/// \brief operator ==
/// \param bs1 - first compared byte symbol.
/// \param bs2 - second compared byte symbol.
/// \return `true if symbols are equal, else `false.
///
template <std::uint8_t numBytes>
bool operator==(const BytesWord<numBytes>& bs1,
                const BytesWord<numBytes>& bs2);

////////////////////////////////////////////////////////////////////////////////
/// \brief operator !=
/// \param bs1 - first compared byte symbol.
/// \param bs2 - second compared byte symbol.
/// \return `true if symbols are not equal, else `false.
///
template <std::uint8_t numBytes>
bool operator!=(const BytesWord<numBytes>& bs1,
                const BytesWord<numBytes>& bs2);

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
template <std::uint8_t _numBytes, class CountT>
std::uint64_t
BytesWord<_numBytes, CountT>::ord(const BytesWord<_numBytes>& word) {
    std::uint64_t ret = 0;
    auto& asBytesArr = reinterpret_cast<std::array<std::byte, 8>&>(ret);
    std::copy(word._data.begin(), word._data.end(), asBytesArr.rend() - _numBytes);
    return ret;
}

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes, class CountT>
auto BytesWord<_numBytes, CountT>::byOrd(
        std::uint64_t ord) -> BytesWord<_numBytes> {
    static_assert(_numBytes < 8, "Big numbers of bytes are not supported.");
    const auto& asBytesOrdArr = reinterpret_cast<std::array<std::byte, 8>&>(ord);
    std::array<std::byte, _numBytes> retBytes;
    std::copy(asBytesOrdArr.rend() - _numBytes, asBytesOrdArr.rend(), retBytes.begin());
    return BytesWord<_numBytes>(retBytes.data());
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes, class CountT>
BytesWord<numBytes, CountT>::BytesWord(const std::byte* ptr) {
    std::memcpy(_data.data(), ptr, numBytes);
}

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes, class CountT>
BytesWord<_numBytes, CountT>::BytesWord(const std::array<std::byte, _numBytes>& arr)
    : BytesWord(arr.data()) {};

//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes, class CountT>
template <std::output_iterator<bool> IterT>
void BytesWord<_numBytes, CountT>::bitsOut(IterT outIter) const {
    for (auto byte: _data) {
        for (auto bit: ga::impl::make_bits_iterator_range(byte)) {
            *outIter = bit;
            ++outIter;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool operator==(const w::BytesWord<numBytes>& bs1,
                const w::BytesWord<numBytes>& bs2) {
    return bs1._data == bs2._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool operator!=(const w::BytesWord<numBytes>& bs1,
                const w::BytesWord<numBytes>& bs2) {
    return bs1._data != bs2._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::ostream& operator<<(std::ostream& os, w::BytesWord<numBytes> sym) {
    for (auto iter = sym._data.begin(); iter < sym._data.end() - 1; ++iter) {
        os << std::bitset<8>(std::to_integer<std::uint8_t>(*iter)) << ' ';
    }
    return os << std::bitset<8>(std::to_integer<std::uint8_t>(*sym._data.rbegin()));
}

}  // namespace ga::w

#endif // BYTES_SYMBOL_HPP
