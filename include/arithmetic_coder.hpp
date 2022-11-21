#pragma once

#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>
#include <vector>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <bitset>

#include "arithmetic_coder_encoded.hpp"
#include "misc.hpp"

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT, typename CountT = std::uint32_t>
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

    CountT _getCumulativeNumFoundLow(const Sym& word);

    CountT _getCumulativeNumFoundHigh(const Sym& word);

    void _serializeNumBytes(Res& res);

    void _serializeTail(Res& res);

    void _serializeAlphabet(Res& res);

private:

    template <class Value>
    using MapSymTo = std::map<Sym, Value, typename Sym::Order>;

private:
    FlowT _symFlow;
    MapSymTo<CountT> _cumulativeNumFound;
    CountT _totalSymsCount;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
garchiever::ArithmeticCoder<FlowT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow) {
    _countProbabilities();
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
auto garchiever::ArithmeticCoder<FlowT, CountT>::encode() -> Res {
    auto ret = Res();

    _serializeNumBytes(ret);
    _serializeTail(ret);
    _serializeAlphabet(ret);

    constexpr auto wordsNum = static_cast<std::uint64_t>(Sym::maxNum);
    constexpr auto wordsNum_2 = wordsNum / 2;
    constexpr auto wordsNum_4 = wordsNum / 4;
    constexpr auto wordsNum_3to4 = 3 * wordsNum / 4;

    std::uint64_t low = 0;
    std::uint64_t high = wordsNum - 1;
    std::size_t btf = 0;

    std::size_t i = 0;

    for (auto sym : _symFlow) {
        std::uint64_t range = high - low + 1;

        auto h = _getCumulativeNumFoundHigh(sym);
        auto l = _getCumulativeNumFoundLow(sym);

        high = low + (range * h) / _totalSymsCount - 1;
        low = low + (range * l) / _totalSymsCount;

        if (high < low) {
            high = low;
        }

        std::cerr << '|';
        std::cerr << i << sym << std::endl;
        ++i;

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
        }
        std::cerr << std::endl;
    }

    std::cerr << "Final bits to follow: " << btf << std::endl;

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
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_countProbabilities() {
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
template <class FlowT, typename CountT>
auto
garchiever::ArithmeticCoder<FlowT, CountT>::_getCumulativeNumFoundLow(
        const Sym &word) -> CountT {
    assert(_cumulativeNumFound.contains(word));
    return _cumulativeNumFound[word];
}


//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
auto
garchiever::ArithmeticCoder<FlowT, CountT>::_getCumulativeNumFoundHigh(
        const Sym &word) -> CountT {
    assert(_cumulativeNumFound.contains(word));
    auto iter = _cumulativeNumFound.find(word);
    if (auto nextIter = std::next(iter); nextIter == _cumulativeNumFound.end()) {
        return _totalSymsCount;
    } else {
        return nextIter->second;
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_serializeNumBytes(Res& res) {
    constexpr std::uint8_t numBytes = Sym::numBytes;
    res.putT<std::uint8_t>(numBytes);
    std::cerr << "Num bytes: "
              << static_cast<unsigned int>(numBytes) << std::endl;
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_serializeTail(Res& res) {
    auto tailSize = static_cast<uint8_t>(_symFlow.getTailSize());
    res.putT<std::uint8_t>(tailSize);
    std::cerr << "Tail size: " << static_cast<unsigned int>(tailSize)
              << std::endl;

    auto tail = _symFlow.getTail();
    for (auto tailByte : tail) {
        res.putByte(tailByte);
        std::cerr << tailByte << ' ';
    }
    std::cerr << std::endl;
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_serializeAlphabet(Res& res) {
    { // Number of unique words
        res.putT<std::uint32_t>(_cumulativeNumFound.size());
        std::cerr << "Number of unique words: "
                  << _cumulativeNumFound.size() << std::endl;
    }

    { // Unique words and their counts
        for (auto [word, cumulFound]: _cumulativeNumFound) {
            res.putT(word);
            std::cerr << "Word: " << word;
            res.putT<CountT>(cumulFound);
            std::cerr << "Count: " << cumulFound << std::endl;
        }
    }

    { // Total words count.
        std::cerr << "Total count: " << _totalSymsCount << std::endl;
        res.putT<CountT>(_totalSymsCount);
    }
}

#endif // ARITHMETIC_CODER_HPP
