#pragma once

#ifndef BIT_SYMBOL_HPP
#define BIT_SYMBOL_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <cassert>
#include <cstring>
#include <ostream>
#include <boost/range/combine.hpp>

#include "misc.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesSymbol class
///
template <std::uint8_t _numBytes>
class BytesSymbol {
public:

    constexpr static std::uint8_t numBytes = _numBytes;
    constexpr static std::uint64_t wordsCount = 1ull << (8 * numBytes);

public:

    static std::uint64_t ord(const BytesSymbol<numBytes>& word);

public:

    BytesSymbol() = default;

    /**
     * @brief BytesSymbol bytes symbol constructor from pointer to data.
     * @param ptr - pointer to data.
     */
    BytesSymbol(std::byte* ptr);

private:
    std::array<std::byte, numBytes> _data;  // data of the word
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The Order class
    ///
    struct Order {
        /**
         * @brief operator ()
         * @param first
         * @param second
         * @return
         */
        bool operator()(const BytesSymbol<numBytes>& s1,
                        const BytesSymbol<numBytes>& s2) const;
    };

private:

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    template <std::uint8_t __numBytes>
    friend bool operator==(const BytesSymbol<__numBytes>& bs1,
                           const BytesSymbol<__numBytes>& bs2);

    /**
     * @brief operator == check if two symbols are equal.
     * @param bs1 - first byte symbol.
     * @param bs2 - second byte symbol.
     */
    template <std::uint8_t __numBytes>
    friend bool operator!=(const BytesSymbol<__numBytes>& bs1,
                           const BytesSymbol<__numBytes>& bs2);

    /**
     * @brief operator << for debug output.
     * @param os - output stream.
     * @return output stream reference.
     */
    template <std::uint8_t __numBytes>
    friend std::ostream& operator<<(std::ostream& os, BytesSymbol<__numBytes>);
};

////////////////////////////////////////////////////////////////////////////////
/// \brief operator ==
/// \param bs1
/// \param bs2
/// \return
///
template <std::uint8_t numBytes>
bool operator==(const BytesSymbol<numBytes>& bs1,
                const BytesSymbol<numBytes>& bs2);

////////////////////////////////////////////////////////////////////////////////
/// \brief operator !=
/// \param bs1
/// \param bs2
/// \return
///
template <std::uint8_t numBytes>
bool operator!=(const BytesSymbol<numBytes>& bs1,
                const BytesSymbol<numBytes>& bs2);


////////////////////////////////////////////////////////////////////////////////
/// \brief operator <<
/// \param os
/// \param sym
/// \return
///
template <std::uint8_t numBytes>
std::ostream& operator<<(std::ostream& os,
                         BytesSymbol<numBytes> sym);


}  // namespace ga

#endif // BIT_SYMBOL_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t _numBytes>
std::uint64_t
ga::BytesSymbol<_numBytes>::ord(const BytesSymbol<numBytes>& word) {
    std::uint64_t ret = 0;
    auto& asBytesArr = reinterpret_cast<std::array<std::byte, 8>&>(ret);
    std::copy(word._data.begin(), word._data.end(), asBytesArr.end() - numBytes);
    return ret;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
ga::BytesSymbol<numBytes>::BytesSymbol(std::byte* ptr) {
    std::memcpy(_data.data(), ptr, numBytes);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool ga::BytesSymbol<numBytes>::Order::operator()(
    const ga::BytesSymbol<numBytes>& s1,
    const ga::BytesSymbol<numBytes>& s2) const {
    for (auto [firstB, secondB] : boost::range::combine(s1._data, s2._data)) {
        std::uint8_t firstU = static_cast<std::uint8_t>(firstB);
        std::uint8_t secondU = static_cast<std::uint8_t>(secondB);
        if (firstU != secondU) {
            return firstU < secondU;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool ga::operator==(const ga::BytesSymbol<numBytes>& bs1,
                    const ga::BytesSymbol<numBytes>& bs2) {
    return bs1._data == bs2._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool ga::operator!=(const ga::BytesSymbol<numBytes>& bs1,
                    const ga::BytesSymbol<numBytes>& bs2) {
    return bs1._data != bs2._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::ostream& ga::operator<<(std::ostream& os,
                             ga::BytesSymbol<numBytes> sym) {
    for (auto iter = sym._data.begin(); iter < sym._data.end() - 1; ++iter) {
        os << *iter << ' ';
    }
    return os << *(sym._data.end() - 1);
}
