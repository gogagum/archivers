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
class ByteDataConstructor {
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The ByteBackInserter class
    ///
    class ByteBackInserter;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BitBackInserter class
    ///
    class BitBackInserter;

public:

    /**
     * @brief ArithmeticCoderEncoded - enpty encoded constructor
     */
    ByteDataConstructor();

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
     * @brief putBitsRepeatWithReset
     * @param bit
     * @param num
     */
    void putBitsRepeatWithReset(bool bit, std::size_t& num);

    /**
     * @brief putByte - add byte in the end
     * @param b - byte
     */
    void putByte(std::byte b);

    /**
     * @brief putT - put object as a byte sequence
     * @param s - object
     */
    void putT(auto s);

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

    /**
     * @brief getBitBackInserter returns bit insert iterator to add bits at
     * the end.
     * @return bit insert iterator.
     */
    BitBackInserter getBitBackInserter();

    /**
     * @brief getByteBackInserter returns byte insert iterator to add bytes at
     * the end.
     * @return
     */
    ByteBackInserter getByteBackInserter();

private:

    void _moveBitFlag();

private:
    std::vector<std::byte> _data;
    std::byte _currBitFlag;
    bool _startedBits;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
void ByteDataConstructor::putT(auto s) {
    using SizeTBytes = std::array<std::byte, sizeof(s)>;

    auto& bytes = reinterpret_cast<SizeTBytes&>(s);
    for (auto byte: boost::adaptors::reverse(bytes)) {
        putByte(byte);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::ByteBackInserter class
///
class ByteDataConstructor::ByteBackInserter {
public:
    using difference_type = std::ptrdiff_t;
public:
    ByteBackInserter(ByteDataConstructor& owner) : _owner(&owner) {};
    ByteBackInserter operator++(int) { return* this; }
    ByteBackInserter& operator++() { return *this; }
    void operator=(std::byte byte) { _owner->putByte(byte); }
    ByteBackInserter& operator*() { return* this; }
private:
    ByteDataConstructor* _owner;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteDataConstructor::BitBackInserter class
///
class ByteDataConstructor::BitBackInserter {
public:
    using difference_type = std::ptrdiff_t;
public:
    BitBackInserter(ByteDataConstructor& owner) : _owner(&owner) {};
    BitBackInserter operator++(int) { return* this; }
    BitBackInserter& operator++() { return *this; }
    void operator=(bool bit) { _owner->putBit(bit); }
    BitBackInserter& operator*() { return *this; }
    ByteBackInserter getBytesIter() { return _owner->getByteBackInserter(); }
private:
    ByteDataConstructor* _owner;
};



} // namespace ga

#endif // ARITHMETIC_CODER_ENCODED_HPP
