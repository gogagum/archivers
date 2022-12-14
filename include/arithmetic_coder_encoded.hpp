#ifndef ARITHMETIC_CODER_ENCODED_HPP
#define ARITHMETIC_CODER_ENCODED_HPP

#include <boost/range/adaptor/reversed.hpp>
#include <vector>
#include <array>
#include <cstdint>
#include <cstddef>
#include <ranges>

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoderEncoded class
///
class ArithmeticCoderEncoded {
public:

    /**
     * @brief ArithmeticCoderEncoded - enpty encoded constructor
     */
    ArithmeticCoderEncoded();

    /**
     * @brief putBit - add single bit in the end.
     * @param bit - a bit to set.
     */
    void putBit(bool bit);

    /**
     * @brief putBitsRepeat
     * @param bit - bit which is set
     * @param num - number of bits repeated
     */
    void putBitsRepeat(bool bit, std::size_t num);

    /**
     * @brief putByte - add byte in the end
     * @param b - byte
     */
    void putByte(std::byte b);

    /**
     * @brief putT - put object as a byte sequence
     * @param s - object
     */
    template <class T>
    void putT(T s);

    /**
     * @brief data - get data pointer
     * @return data pointer
     */
    const std::byte* data() const;

    /**
     * @brief bytesSize - get data bytes size
     * @return data size in bytes
     */
    const std::size_t bytesSize() const;

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::byte _currBitFlag;
    bool _startedBits;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class T>
void ArithmeticCoderEncoded::putT(T s) {
    using SizeTBytes = std::array<std::byte, sizeof(T)>;

    static_assert(sizeof(SizeTBytes) == sizeof(T), "Error with sizes.");

    auto& bytes = reinterpret_cast<SizeTBytes&>(s);
    for (auto byte: boost::adaptors::reverse(bytes)) {
        putByte(byte);
    }
}



} // namespace ga

#endif // ARITHMETIC_CODER_ENCODED_HPP
