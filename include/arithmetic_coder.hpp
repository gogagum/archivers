#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>
#include <vector>
#include <cassert>
#include <iostream>
#include <bitset>

#include "arithmetic_coder_encoded.hpp"
#include "misc.hpp"

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT>
class ArithmeticCoder {
public:

    using Sym = typename FlowT::Sym;

    using Res = ArithmeticCoderEncoded;

public:

    /**
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
    ArithmeticCoder(FlowT&& byteFlow);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    Res encode();

private:

    void _countProbabilities();

    std::uint64_t _getCumulativeNumFoundLow(const Sym& word);

    std::uint64_t _getCumulativeNumFoundHigh(const Sym& word);

private:

    template <class Value>
    using MapSymTo = std::map<Sym, Value, typename Sym::Order>;

private:
    FlowT _symFlow;
    MapSymTo<std::int64_t> _cumulativeNumFound;
    std::int64_t _totalSymsCount;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT>
garchiever::ArithmeticCoder<FlowT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow) {
    _countProbabilities();
}

//----------------------------------------------------------------------------//
template <class FlowT>
auto garchiever::ArithmeticCoder<FlowT>::encode() -> Res {
    auto ret = Res();

    constexpr std::uint8_t numBytes = Sym::numBytes;
    ret.putT<std::uint8_t>(numBytes);
    std::cerr << "Num bytes: " << static_cast<unsigned int>(numBytes) << std::endl;
    auto tailSize = static_cast<uint8_t>(_symFlow.getTailSize());
    ret.putT<std::uint8_t>(tailSize);
    std::cerr << "Tail size: " << static_cast<unsigned int>(tailSize) << std::endl;
    auto tail = _symFlow.getTail();
    for (auto tailByte : tail) {
        ret.putByte(tailByte);
        std::cerr << tailByte << ' ';
    }
    std::cerr << std::endl;
    ret.putT(static_cast<uint64_t>(_cumulativeNumFound.size()));
    std::cerr << "Number of unique words: "
              << _cumulativeNumFound.size() << std::endl;
    for (auto [word, cumulFound]: _cumulativeNumFound) {
        ret.putT(word);
        std::cerr << "Word: " << word;
        ret.putT(_cumulativeNumFound[word]);
        std::cerr << "Count: " << _cumulativeNumFound[word] << std::endl;
    }
    std::cerr << "Total count: " << _totalSymsCount << std::endl;
    ret.putT(static_cast<uint64_t>(_totalSymsCount));

    constexpr auto wordsNum = static_cast<std::uint64_t>(Sym::maxNum);
    constexpr auto wordsNum_2 = wordsNum / 2;
    constexpr auto wordsNum_4 = wordsNum / 4;
    constexpr auto wordsNum_3to4 = 3 * wordsNum / 4;

    std::uint64_t low = 0;
    std::uint64_t high = wordsNum - 1;
    std::size_t btf = 0;
std::cout << "hoho\n";
    for (auto sym : _symFlow) {
        std::uint64_t range = high - low + 1;

        auto h = _getCumulativeNumFoundHigh(sym);
        auto l = _getCumulativeNumFoundLow(sym);

        high = low + (range * _getCumulativeNumFoundHigh(sym)) / _totalSymsCount - 1;
        low = low + (range * _getCumulativeNumFoundLow(sym)) / _totalSymsCount;

        if (high < low) {
            high = low;
        }

        while (true) {
            if (high < wordsNum_2) {
                ret.putBit(false);
                ret.putBitsRepeat(true, btf);
                btf = 0;
                high = high * 2 + 1;
                low = low * 2;
            } else if (low >= wordsNum_2) {
                ret.putBit(true);
                ret.putBitsRepeat(false, btf);
                btf = 0;
                high = high * 2 - wordsNum + 1;
                low = low * 2 - wordsNum;
            } else if (low >= wordsNum_4 && high < wordsNum_3to4) {
                high = high * 2 - wordsNum_2 + 1;
                low = low * 2 - wordsNum_2;
                ++btf;
            } else {
                break;
            }
            assert(high >= low);
        }
    }

    if (low < wordsNum_4) {
        ret.putBit(false);
        ret.putBitsRepeat(true, btf + 1);
    } else {
        ret.putBit(true);
        ret.putBitsRepeat(false, btf + 1);
    }

    return ret;
}

//----------------------------------------------------------------------------//
template <class FlowT>
void garchiever::ArithmeticCoder<FlowT>::_countProbabilities() {
    MapSymTo<std::uint64_t> numFound;
    for (auto word : _symFlow) {
        numFound[word]++;
    }
    _totalSymsCount = 0;
    for (auto [word, num] : numFound) {
        _cumulativeNumFound[word] = _totalSymsCount;
        _totalSymsCount += num;
    }
}

//----------------------------------------------------------------------------//
template <class FlowT>
std::uint64_t
garchiever::ArithmeticCoder<FlowT>::_getCumulativeNumFoundLow(const Sym &word) {
    assert(_cumulativeNumFound.contains(word));
    return _cumulativeNumFound[word];
}


//----------------------------------------------------------------------------//
template <class FlowT>
std::uint64_t
garchiever::ArithmeticCoder<FlowT>::_getCumulativeNumFoundHigh(const Sym &word) {
    assert(_cumulativeNumFound.contains(word));
    auto iter = _cumulativeNumFound.find(word);
    if (auto nextIter = std::next(iter); nextIter == _cumulativeNumFound.end()) {
        return _totalSymsCount;
    } else {
        return nextIter->second;
    }
}

#endif // ARITHMETIC_CODER_HPP
