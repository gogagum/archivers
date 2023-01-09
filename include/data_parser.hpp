#pragma once

#ifndef ARITHMETIC_DECODER_DECODED_HPP
#define ARITHMETIC_DECODER_DECODED_HPP

#include <boost/range/adaptor/reversed.hpp>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <array>
#include <ranges>

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoderDecoded class
///
class DataParser {
public:

    /**
     * @brief ArithmeticDecoderDecoded
     * @param data
     */
    DataParser(std::vector<std::byte>&& data);

    /**
     * @brief takeByte
     * @return
     */
    std::byte takeByte();

    /**
     * @brief takeBit
     * @return
     */
    bool takeBit();

    /**
     * @brief takeT
     * @return
     */
    template <class T>
    T takeT();

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::size_t _bytesRead;
    std::byte _currBitFlag;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class T>
T DataParser::takeT() {
    using TBytes = std::array<std::byte, sizeof(T)>;

    auto ret = T();
    auto& bytes = reinterpret_cast<TBytes&>(ret);
    for (auto& byte: boost::adaptors::reverse(bytes)) {
        byte = takeByte();
    }
    return ret;
}

}  // namespace ga

#endif  // ARITHMETIC_DECODER_DECODED_HPP
