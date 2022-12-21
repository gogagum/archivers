#ifndef INT_RANGE_WORD_HPP
#define INT_RANGE_WORD_HPP

#include <ostream>
#include <cassert>
#include <cstdint>

namespace ga::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerWord class
///
template <class I, I low, I high>
class IntegerWord {
public:

    constexpr static std::uint64_t wordsCount = static_cast<std::uint64_t>(high - low);

public:

    /**
     * @brief ord
     * @param word
     * @return
     */
    static std::uint64_t ord(const IntegerWord<I, low, high>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static IntegerWord<I, low, high> byOrd(std::uint64_t ord);

public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The BytesAfterBitsException class
    ///
    class IncorrectOrd : public std::runtime_error {
    public:
        IncorrectOrd();
    };

public:
    IntegerWord(I value);

private:
    I _value;

private:
    template <class _I, _I _low, _I _high>
    friend bool operator==(const IntegerWord<_I, _low, _high>& iw1,
                           const IntegerWord<_I, _low, _high>& iw2);


    template <class _I, _I _low, _I _high>
    friend bool operator!=(const IntegerWord<_I, _low, _high>& iw1,
                           const IntegerWord<_I, _low, _high>& iw2);


    friend std::ostream& operator<<(std::ostream& os, IntegerWord<I, low, high>& word);
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, I high>
std::uint64_t IntegerWord<I, low, high>::ord(const IntegerWord<I, low, high>& word) {
    return static_cast<std::uint64_t>(word._value - low);
}

//----------------------------------------------------------------------------//
template <class I, I low, I high>
IntegerWord<I, low, high> IntegerWord<I, low, high>::byOrd(std::uint64_t ord) {
    return IntegerWord<I, low, high>(static_cast<I>(ord + low));
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, I high>
IntegerWord<I, low, high>::IntegerWord(I value) : _value(value) {
    if (_value < low || _value >= high) {
        throw IncorrectOrd();
    }
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, I high>
IntegerWord<I, low, high>::IncorrectOrd::IncorrectOrd() : std::runtime_error("Incorrect ord.") {}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, I high>
std::ostream& operator<<(std::ostream& os, IntegerWord<I, low, high>& word) {
    os << "Word(" << word._value - low << ")";
}

//----------------------------------------------------------------------------//
template <class I, I low, I high>
bool operator==(const IntegerWord<I, low, high>& iw1,
                const IntegerWord<I, low, high>& iw2) {
    return iw1._value == iw2._value;
}

//----------------------------------------------------------------------------//
template <class I, I low, I high>
bool operator!=(const IntegerWord<I, low, high>& iw1,
                const IntegerWord<I, low, high>& iw2) {
    return iw1._value != iw2._value;
}

}

#endif // INT_RANGE_WORD_HPP
