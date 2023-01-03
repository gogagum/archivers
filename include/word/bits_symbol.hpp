#ifndef BITS_SYMBOL_HPP
#define BITS_SYMBOL_HPP

#include <array>
#include <algorithm>
#include <concepts>
#include <cstdint>

namespace ga::w{

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWord class
///
template <std::uint16_t _numBits>
class BitsWord {
public:

    constexpr static std::uint16_t numBits = _numBits;
    constexpr static std::uint64_t wordsCount = 1ull << numBits;

public:

    /**
     * @brief ord - get word index in order of words.
     * @param word - to get order of.
     * @return word order index.
     */
    static std::uint64_t ord(const BitsWord<_numBits>& word);

    /**
     * @brief byOrd
     * @param ord
     * @return
     */
    static BitsWord<_numBits> byOrd(std::uint64_t ord);

public:

    /**
     * Default constructor. Constructs a symbol filled with zeros,
     */
    BitsWord() = default;

    /**
     * @brief BitsWord - constructor from bits input iterator.
     * @param inputIter - input iterator to take bits form.
     */
    template <std::input_iterator IterT>
    BitsWord(IterT& inputIter);

    /**
     * @brief bitsOut - give bits of a word out.
     * @param outIter - outPut bits iterator to erite bits.
     */
    template <std::output_iterator<bool> IterT>
    void bitsOut(IterT outIter) const;

private:
    std::array<bool, numBits> _bits;

};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
template <std::input_iterator IterT>
BitsWord<_numBits>::BitsWord(IterT& iter) {
    for (auto& bit : _bits) {
        bit = *iter;
        ++iter;
    }
}

//----------------------------------------------------------------------------//
template <std::uint16_t _numBits>
template <std::output_iterator<bool> IterT>
void BitsWord<_numBits>::bitsOut(IterT outIter) const {
    std::ranges::copy(_bits, outIter);
}

}

#endif // BITS_SYMBOL_HPP
