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
template <std::size_t numBytes>
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
                        const BytesSymbol<numBytes>& s2);
    };
};

}  // namespace garcheiver


////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::size_t numBytes>
garchiever::BytesSymbol<numBytes>::BytesSymbol(std::byte* ptr) {
    std::memcpy(_data.data(), ptr, numBytes);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::size_t numBytes>
bool garchiever::BytesSymbol<numBytes>::Order::operator()(
    const garchiever::BytesSymbol<numBytes>& s1,
    const garchiever::BytesSymbol<numBytes>& s2) {
    for (auto [first, second] : boost::range::combine(s1._data, s2._data)) {
        if (static_cast<std::uint8_t>(*first)
                < static_cast<std::uint8_t>(*second)) {
            return true;
        }
    }
    return false;
}

#endif // BIT_SYMBOL_HPP
