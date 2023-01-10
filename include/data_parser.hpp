#pragma once

#ifndef ARITHMETIC_DECODER_DECODED_HPP
#define ARITHMETIC_DECODER_DECODED_HPP

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>

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
    explicit DataParser(std::span<std::byte> data);

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

    /**
     * @brief getNumBytes - get number of bytes parsed.
     * @return number of bytes.
     */
    std::size_t getNumBytes() const;

    /**
     * @brief seek move to bitsOffset position.
     * @param bitsOffset - position to move to.
     */
    void seek(std::size_t bitsOffset);

private:

    void _moveBitFlag();

private:
    const std::span<std::byte> _data;
    std::span<std::byte>::iterator _dataIter;
    std::byte _currBitFlag;

private:

    friend bool operator==(const DataParser& dp1, const DataParser& dp2);

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
bool operator==(const DataParser& dp1, const DataParser& dp2);

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
