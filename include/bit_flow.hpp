#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include "bit_symbol.hpp""

#include <vector>
#include <cstddef>
#include <cstring>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitFlow class
class BitFlow {
public:
    BitFlow(const std::byte* ptr, std::size_t numBytes);

    template <std::size_t num>
    BitSymbol<num> takeBitSymbol();

private:
    std::vector<std::byte> _bytes;
};
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
garchiever::BitFlow::BitFlow(const std::byte* ptr, std::size_t numBytes) {
    _bytes.resize(numBytes);
    std::memcpy(_bytes.data(), ptr, numBytes);
}

#endif // BIT_FLOW_HPP
