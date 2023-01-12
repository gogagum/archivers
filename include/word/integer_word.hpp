#ifndef INT_RANGE_WORD_HPP
#define INT_RANGE_WORD_HPP

#include <concepts>
#include <ostream>
#include <cassert>
#include <cstdint>

#include "ord_t_choose.hpp"

namespace ga::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerWord class
///
template <
    std::integral I, I low, std::uint16_t _numBits,
    typename CountT = std::uint64_t
>
class IntegerWord {
public:

    using Ord = typename impl::OrdTChoose<_numBits>::Type;
    using Count = CountT;

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
    ord(const IntegerWord<I, low, _numBits, CountT>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static IntegerWord<I, low, _numBits, CountT> byOrd(std::uint64_t ord);

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
    template <
        std::integral _I, _I _low, std::uint16_t __numBits, typename _CountT
    >
    friend bool operator==(
        const IntegerWord<_I, _low, __numBits, _CountT>& iw1,
        const IntegerWord<_I, _low, __numBits, _CountT>& iw2
    );

    template <
        std::integral _I, _I _low, std::uint16_t __numBits, class _CountT
    >
    friend bool operator!=(
        const IntegerWord<_I, _low, __numBits, _CountT>& iw1,
        const IntegerWord<_I, _low, __numBits, _CountT>& iw2
    );

    template <
        std::integral _I, _I _low, std::uint16_t __numBits, class _CountT
    >
    friend std::ostream& operator<<(
        std::ostream& os,
        IntegerWord<I, low, _numBits, _CountT>& word
    );
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
std::uint64_t
IntegerWord<I, low, _numBits, CountT>::ord(
        const IntegerWord<I, low, _numBits, CountT>& word) {
    return static_cast<std::uint64_t>(word._value - low);
}

//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
IntegerWord<I, low, _numBits, CountT>
IntegerWord<I, low, _numBits, CountT>::byOrd(std::uint64_t ord) {
    return IntegerWord<I, low, _numBits, CountT>(
                static_cast<I>(ord + low));
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
IntegerWord<I, low, _numBits, CountT>::IntegerWord(I value) : _value(value) {
    if (_value < low || _value >= high) {
        throw IncorrectOrd(_value);
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
IntegerWord<I, low, _numBits, CountT>::IncorrectOrd::IncorrectOrd(I v)
    : std::runtime_error("low: " + std::to_string(low)
                         + " high: " + std::to_string(high)
                         + " value: " + std::to_string(v) ) {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
std::ostream&
operator<<(std::ostream& os,
           IntegerWord<I, low, _numBits, CountT>& word) {
    os << "Word(" << word._value - low << ")";
    return os;
}

//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
bool operator==(const IntegerWord<I, low, _numBits, CountT>& iw1,
                const IntegerWord<I, low, _numBits, CountT>& iw2) {
    return iw1._value == iw2._value;
}

//----------------------------------------------------------------------------//
template <
    std::integral I, I low, std::uint16_t _numBits, typename CountT
>
bool operator!=(const IntegerWord<I, low, _numBits, CountT>& iw1,
                const IntegerWord<I, low, _numBits, CountT>& iw2) {
    return iw1._value != iw2._value;
}

}

#endif // INT_RANGE_WORD_HPP
