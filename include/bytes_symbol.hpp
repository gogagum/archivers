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

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesSymbol class
///
template <std::uint8_t _numBytes>
class BytesSymbol {
public:

    constexpr static std::uint8_t numBytes = _numBytes;
    constexpr static std::uint64_t maxNum = std::uint64_t(1) << (8 * numBytes);

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
     * @brief operator <<
     * @param os
     * @return
     */
    template <std::uint8_t __numBytes>
    friend std::ostream& operator<<(std::ostream& os, BytesSymbol<__numBytes>);
};

template <std::uint8_t numBytes>
std::ostream& operator<<(std::ostream& os,
                         BytesSymbol<numBytes> sym);


}  // namespace garcheiver

#endif // BIT_SYMBOL_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
garchiever::BytesSymbol<numBytes>::BytesSymbol(std::byte* ptr) {
    std::memcpy(_data.data(), ptr, numBytes);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool garchiever::BytesSymbol<numBytes>::Order::operator()(
    const garchiever::BytesSymbol<numBytes>& s1,
    const garchiever::BytesSymbol<numBytes>& s2) const {
    for (auto [firstB, secondB] : boost::range::combine(s1._data, s2._data)) {
        std::uint8_t firstU = static_cast<std::uint8_t>(firstB);
        std::uint8_t secondU = static_cast<std::uint8_t>(secondB);
        if (firstU < secondU) {
            return true;
        } else if (firstU > secondU) {
            return false;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool operator==(const garchiever::BytesSymbol<numBytes>& bs1,
                const garchiever::BytesSymbol<numBytes>& bs2) {
    return bs1._data == bs2._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
bool operator!=(const garchiever::BytesSymbol<numBytes>& bs1,
                const garchiever::BytesSymbol<numBytes>& bs2) {
    return bs1._data != bs2._data;
}

//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
std::ostream& garchiever::operator<<(std::ostream& os,
                                     garchiever::BytesSymbol<numBytes> sym) {
    for (auto iter = sym._data.begin(); iter < sym._data.end() - 1; ++iter) {
        os << *iter << ' ';
    }
    return os << *(sym._data.end() - 1);
}
