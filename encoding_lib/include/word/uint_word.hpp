#ifndef UINT_WORD_HPP
#define UINT_WORD_HPP

#include <concepts>
#include <cstdint>

namespace ga::w {

template <std::integral CountT>
class UIntWord {
public:

    using Ord = CountT;
    using Count = CountT;

public:

    constexpr static std::uint16_t numBits = sizeof(CountT) * 8;
    constexpr static std::uint64_t wordsCount = 1ull << numBits;

public:

    /**
     * @brief ord
     * @param word
     * @return
     */\
    static Ord ord(const UIntWord<CountT>& word) { return word._value; };

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static UIntWord<Count> byOrd(Ord ord) { return UIntWord<Count>(ord); };

public:

    /**
     * @brief IntegerWord - constructor from value.
     * @param value
     */
    explicit UIntWord(CountT value) : _value(value) {}

    /**
     * @brief getValue
     * @return value of word.
     */
    CountT getValue() const { return _value; }

private:

    const CountT _value;

private:
    friend bool operator==(const UIntWord&, const UIntWord&) = default;
};


}

#endif // UINT_WORD_HPP
