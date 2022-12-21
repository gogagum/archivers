#ifndef INT_RANGE_WORD_HPP
#define INT_RANGE_WORD_HPP

#include <ostream>
#include <cassert>
#include <cstdint>

namespace ga::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerWord class
///
template <class I, I low, std::uint8_t _numBytes>
class IntegerWord {
public:

    constexpr static std::uint8_t numBytes = _numBytes;
    constexpr static std::uint16_t numBits = _numBytes * 8;
    constexpr static std::uint64_t wordsCount = 1ull << numBits;
    constexpr static std::int64_t high = low + wordsCount;

public:

    /**
     * @brief ord
     * @param word
     * @return
     */
    static std::uint64_t ord(const IntegerWord<I, low, _numBytes>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static IntegerWord<I, low, _numBytes> byOrd(std::uint64_t ord);

public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BytesAfterBitsException class
    ///
    class IncorrectOrd : public std::runtime_error {
    public:
        IncorrectOrd(I v);
    };

public:
    IntegerWord(I value);

private:
    I _value;

private:
    template <class _I, _I _low, std::uint8_t __numBytes>
    friend bool operator==(const IntegerWord<_I, _low, __numBytes>& iw1,
                           const IntegerWord<_I, _low, __numBytes>& iw2);


    template <class _I, _I _low, std::uint8_t __numBytes>
    friend bool operator!=(const IntegerWord<_I, _low, __numBytes>& iw1,
                           const IntegerWord<_I, _low, __numBytes>& iw2);


    friend std::ostream& operator<<(std::ostream& os, IntegerWord<I, low, _numBytes>& word);
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
std::uint64_t IntegerWord<I, low, _numBytes>::ord(const IntegerWord<I, low, _numBytes>& word) {
    return static_cast<std::uint64_t>(word._value - low);
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
IntegerWord<I, low, _numBytes> IntegerWord<I, low, _numBytes>::byOrd(std::uint64_t ord) {
    return IntegerWord<I, low, _numBytes>(static_cast<I>(ord + low));
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
IntegerWord<I, low, _numBytes>::IntegerWord(I value) : _value(value) {
    if (_value < low || _value >= high) {
        throw IncorrectOrd(_value);
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
IntegerWord<I, low, _numBytes>::IncorrectOrd::IncorrectOrd(I v)
    : std::runtime_error("low: " + std::to_string(low)
                         + " high: " + std::to_string(high)
                         + " value: " + std::to_string(v) ) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
std::ostream& operator<<(std::ostream& os, IntegerWord<I, low, _numBytes>& word) {
    os << "Word(" << word._value - low << ")";
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
bool operator==(const IntegerWord<I, low, _numBytes>& iw1,
                const IntegerWord<I, low, _numBytes>& iw2) {
    return iw1._value == iw2._value;
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint8_t _numBytes>
bool operator!=(const IntegerWord<I, low, _numBytes>& iw1,
                const IntegerWord<I, low, _numBytes>& iw2) {
    return iw1._value != iw2._value;
}

}

#endif // INT_RANGE_WORD_HPP
