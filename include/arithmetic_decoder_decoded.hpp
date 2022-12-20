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
class ArithmeticDecoderDecoded {
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BytesAfterBitsException class
    ///
    class BytesAfterBitsException : public std::runtime_error {
    public:
        BytesAfterBitsException();
    };

public:

    /**
     * @brief ArithmeticDecoderDecoded
     * @param data
     */
    ArithmeticDecoderDecoded(std::vector<std::byte>&& data);

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
     * @brief getSymNumBytes
     * @return
     */
    uint8_t getSymNumBytes() const;

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::size_t _bytesRead;
    bool _startedBits;
    std::byte _currBitFlag;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class T>
T ArithmeticDecoderDecoded::takeT() {
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
