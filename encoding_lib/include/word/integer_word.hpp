#ifndef INT_RANGE_WORD_HPP
#define INT_RANGE_WORD_HPP

#include <concepts>
#include <ostream>
#include <cassert>
#include <cstdint>

namespace ga::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerWord class
///
template <std::integral I, I low, std::uint16_t _numBits>
class IntegerWord {
public:

    using Ord = std::uint64_t;

public:

    constexpr static std::uint16_t numBits = _numBits;
    constexpr static std::uint64_t wordsCount = 1ull << numBits;
    constexpr static std::int64_t high = low + wordsCount;

public:

    /**
     * @brief ord
     * @param word
     * @return
     */
    static std::uint64_t
    ord(const IntegerWord<I, low, _numBits>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static IntegerWord<I, low, _numBits> byOrd(std::uint64_t ord);

public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BytesAfterBitsException class
    ///
    class IncorrectOrd : public std::runtime_error {
    public:
        IncorrectOrd(I v);
    };

public:

    /**
     * @brief IntegerWord - constructor from value.
     * @param value
     */
    IntegerWord(I value);

    /**
     * @brief getValue
     * @return value of word.
     */
    I getValue() const { return _value + low; }

private:

    const I _value;

private:

    friend bool operator==(const IntegerWord& w1, const IntegerWord& w2) = default;
    friend bool operator!=(const IntegerWord& w1, const IntegerWord& w2) = default;

    template <std::integral _I, _I _low, std::uint16_t __numBits>
    friend std::ostream& operator<<(
        std::ostream& os,
        IntegerWord<I, low, _numBits>& word
    );
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::integral I, I low, std::uint16_t _numBits>
std::uint64_t
IntegerWord<I, low, _numBits>::ord(const IntegerWord<I, low, _numBits>& word) {
    return static_cast<std::uint64_t>(word._value - low);
}

//----------------------------------------------------------------------------//
template <std::integral I, I low, std::uint16_t _numBits>
IntegerWord<I, low, _numBits>
IntegerWord<I, low, _numBits>::byOrd(std::uint64_t ord) {
    return IntegerWord<I, low, _numBits>(
                static_cast<I>(ord + low));
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::integral I, I low, std::uint16_t _numBits>
IntegerWord<I, low, _numBits>::IntegerWord(I value) : _value(value) {
    if (_value < low || _value >= high) {
        throw IncorrectOrd(_value);
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::integral I, I low, std::uint16_t _numBits>
IntegerWord<I, low, _numBits>::IncorrectOrd::IncorrectOrd(I v)
    : std::runtime_error("low: " + std::to_string(low)
                         + " high: " + std::to_string(high)
                         + " value: " + std::to_string(v) ) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::integral I, I low, std::uint16_t _numBits>
std::ostream&
operator<<(std::ostream& os, IntegerWord<I, low, _numBits>& word) {
    os << "Word(" << word._value - low << ")";
    return os;
}

}

#endif // INT_RANGE_WORD_HPP
