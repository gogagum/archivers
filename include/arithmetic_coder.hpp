#pragma once

#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>
#include <vector>
#include <cassert>
#include <cstdint>
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

    constexpr static auto symsNum = static_cast<std::uint64_t>(Sym::maxNum);
    constexpr static auto symsNum_2 = symsNum / 2;
    constexpr static auto symsNum_4 = symsNum / 4;
    constexpr static auto symsNum_3to4 = 3 * symsNum / 4;

private:

    template <class Value>
    using MapSymTo = std::map<Sym, Value, typename Sym::Order>;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The SymsData class
    ///
    struct SymsData {
        MapSymTo<CountT> cumulativeNumFound;
        CountT totalSymsCount;
    };

private:

    SymsData _countSymsData();

    CountT _getCumulativeNumFoundLow(const Sym& word);

    CountT _getCumulativeNumFoundHigh(const Sym& word);

    void _serializeNumBytes(Res& res);

    void _serializeTail(Res& res);

    void _serializeAlphabet(Res& res);

    std::uint64_t _computeCorrectingConst() const;

private:
    FlowT _symFlow;
    SymsData _symsData;
    const std::uint64_t _correctingConst;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
garchiever::ArithmeticCoder<FlowT, CountT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _symFlow(symbolsFlow),
          _correctingConst(_computeCorrectingConst()),
          _symsData(_countSymsData()) {}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
auto garchiever::ArithmeticCoder<FlowT, CountT>::encode() -> Res {
    auto ret = Res();

    _serializeNumBytes(ret);
    _serializeTail(ret);
    _serializeAlphabet(ret);

    std::uint64_t low = 0;
    std::uint64_t high = symsNum * _correctingConst;
    std::size_t btf = 0;

    for (auto sym : _symFlow) {
        std::uint64_t range = high - low;

        auto h = _getCumulativeNumFoundHigh(sym);
        auto l = _getCumulativeNumFoundLow(sym);

        high = low + (range * h) / _symsData.totalSymsCount;
        low = low + (range * l) / _symsData.totalSymsCount;

        while (true) {
            if (high <= symsNum_2 * _correctingConst) {
                ret.putBit(false);
                ret.putBitsRepeat(true, btf);
                btf = 0;
                high = high * 2;
                low = low * 2;
            } else if (low >= symsNum_2 * _correctingConst) {
                ret.putBit(true);
                ret.putBitsRepeat(false, btf);
                btf = 0;
                high = high * 2 - symsNum * _correctingConst;
                low = low * 2 - symsNum * _correctingConst;
            } else if (low >= symsNum_4 * _correctingConst
                       && high <= symsNum_3to4 * _correctingConst) {
                high = high * 2 - symsNum_2 * _correctingConst;
                low = low * 2 - symsNum_2 * _correctingConst;
                ++btf;
            } else {
                break;
            }
        }
    }

    if (low < symsNum_4 * _correctingConst) {
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
auto garchiever::ArithmeticCoder<FlowT, CountT>::_countSymsData() -> SymsData {
    SymsData ret;
    MapSymTo<std::uint64_t> numFound;
    for (auto word : _symFlow) {
        numFound[word]++;
    }
    ret.totalSymsCount = 0;
    for (auto [word, num] : numFound) {
        ret.cumulativeNumFound[word] = ret.totalSymsCount;
        ret.totalSymsCount += num;
    }
    return ret;
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
auto
garchiever::ArithmeticCoder<FlowT, CountT>::_getCumulativeNumFoundLow(
        const Sym &word) -> CountT {
    assert(_symsData.cumulativeNumFound.contains(word));
    return _symsData.cumulativeNumFound[word];
}


//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
auto
garchiever::ArithmeticCoder<FlowT, CountT>::_getCumulativeNumFoundHigh(
        const Sym &word) -> CountT {
    assert(_symsData.cumulativeNumFound.contains(word));
    auto iter = _symsData.cumulativeNumFound.find(word);
    if (auto nextIter = std::next(iter);
            nextIter == _symsData.cumulativeNumFound.end()) {
        return _symsData.totalSymsCount;
    } else {
        return nextIter->second;
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_serializeNumBytes(Res& res) {
    constexpr std::uint8_t numBytes = Sym::numBytes;
    res.putT<std::uint8_t>(numBytes);
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_serializeTail(Res& res) {
    auto tailSize = static_cast<uint8_t>(_symFlow.getTailSize());
    res.putT<std::uint8_t>(tailSize);

    auto tail = _symFlow.getTail();
    for (auto tailByte : tail) {
        res.putByte(tailByte);
    }
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
void garchiever::ArithmeticCoder<FlowT, CountT>::_serializeAlphabet(Res& res) {
    // Number of unique words
    res.putT<std::uint32_t>(_symsData.cumulativeNumFound.size());

    // Unique words and their counts
    for (auto [sym, cumulFound]: _symsData.cumulativeNumFound) {
        res.putT(sym);
        res.putT<CountT>(cumulFound);
    }

    // Total words count.
    res.putT<CountT>(_symsData.totalSymsCount);
}

//----------------------------------------------------------------------------//
template <class FlowT, typename CountT>
std::uint64_t
garchiever::ArithmeticCoder<FlowT, CountT>::_computeCorrectingConst() const {
    std::uint64_t correctingConst = 1;
    while (correctingConst * symsNum < (std::uint64_t{1} << 40)) {
        correctingConst <<= 1;
    }
    return correctingConst;
}


#endif // ARITHMETIC_CODER_HPP
