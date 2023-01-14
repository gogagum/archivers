#pragma once

#ifndef ARITHMETIC_DECODER_DECODED_HPP
#define ARITHMETIC_DECODER_DECODED_HPP

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/iterator_range.hpp>

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoderDecoded class
///
class DataParser {
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BitIterator class
    class BitsIterator;

public:

    /**
     * @brief ArithmeticDecoderDecoded
     * @param data
     */
    explicit DataParser(std::span<const std::byte> data);

    /**
     * @brief takeByte take one byte.
     * @return one byte.
     */
    std::byte takeByte();

    /**
     * @brief takeBit get one bit from current position and move by one bit.
     * @return one bit.
     */
    bool takeBit();

    /**
     * @brief takeT get T as sizeof(T) bits.
     * @return object of type T.
     */
    template <class T>
    T takeT();

    /**
     * @brief getNumBytes - get number of bytes parsed.
     * @return number of bytes.
     */
    std::size_t getNumBytes() const;

    /**
     * @brief getNumBits - get number of bits.
     * @return number of bits.
     */
    std::size_t getNumBits() const;

    /**
     * @brief seek move to bitsOffset position.
     * @param bitsOffset - position to move to.
     */
    void seek(std::size_t bitsOffset);

    BitsIterator getCurrPosBitsIter();

    BitsIterator getEndBitsIter();

    boost::iterator_range<BitsIterator> getCurrTailRange();

private:

    void _moveInByteOffset();

    std::byte _getByteFlag() const;

private:

    const std::span<const std::byte> _data;
    std::span<const std::byte>::iterator _dataIter;
    std::uint8_t _inByteOffset;

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

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
class DataParser::BitsIterator : public boost::iterators::iterator_facade<
            BitsIterator,
            bool,
            boost::single_pass_traversal_tag,
            bool
        > {
public:
    using type = BitsIterator;
public:
    //------------------------------------------------------------------------//
    BitsIterator(DataParser& owner, std::size_t bitsPosition)
        : _owner(&owner), _bitsPosition(bitsPosition) {}
    //------------------------------------------------------------------------//
    bool dereference() const;
    //------------------------------------------------------------------------//
    bool equal(const type& other) const;
    //------------------------------------------------------------------------//
    void increment()                    { ++_bitsPosition; }
private:
    DataParser* _owner;
    std::size_t _bitsPosition;
private:
    friend class boost::iterators::iterator_core_access;
};

}  // namespace ga

#endif  // ARITHMETIC_DECODER_DECODED_HPP
