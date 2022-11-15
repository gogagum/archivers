#ifndef BIT_SYMBOL_HPP
#define BIT_SYMBOL_HPP

#include <cstdint>
#include <array>
#include <cassert>
#include <cstring>
#include <boost/range/combine.hpp>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BytesSymbol class
///
template <std::uint8_t numBytes>
class BytesSymbol {
public:

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
    friend bool operator==(const BytesSymbol<numBytes>& bs1,
                           const BytesSymbol<numBytes>& bs2);
};

}  // namespace garcheiver


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

#endif // BIT_SYMBOL_HPP
