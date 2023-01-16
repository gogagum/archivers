#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/container/static_vector.hpp>
#include <boost/range/irange.hpp>

#include <iostream>
#include <map>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>

#include "data_parser.hpp"
#include "ranges_calc.hpp"
#include "dictionary/traits.hpp"

namespace ga {

namespace bc = boost::container;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class SymT, class DictT, typename CountT = std::uint32_t>
class ArithmeticDecoder : RangesCalc<SymT> {
public:
    using Source = DataParser;
    using Tail = bc::static_vector<bool, SymT::numBits>;

public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The Ret class
    ///
    struct Ret {
        std::vector<SymT> syms;
        Tail tail;
    };

public:

    template <class DictConstructor>
    ArithmeticDecoder(Source& source, DictConstructor&& constructor);

    /**
     * @brief decode - decode source as a vector of bytes.
     * @return
     */
    Ret decode();

private:

    using RangesCalc<SymT>::symsNum;
    using RangesCalc<SymT>::correctedSymsNum;
    using RangesCalc<SymT>::correctedSymsNum_2;
    using RangesCalc<SymT>::correctedSymsNum_4;
    using RangesCalc<SymT>::correctedSymsNum_3to4;
    using OrdRange = typename RangesCalc<SymT>::Range;

private:

    Tail _deserializeTail();

    std::vector<std::uint64_t> _deserializeWordsCounts();

    CountT _deserializeFileWordsCount();

    std::uint8_t _computeAdditionalBitsCnt() const;

private:
    Source& _source;
    DictT _dict;                                 // #1 to deserialize.
    CountT _fileWordsCount;                      // #2 to deserialize.
    const Tail _tail;                            // #3 to deserialize.
    const std::uint8_t _additionalBitsCnt;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
template <class DictConstructor>
ArithmeticDecoder<SymT, DictT, CountT>::ArithmeticDecoder(
        Source& source, DictConstructor&& constructor)
    : _source(source),
      _dict(constructor()),
      _fileWordsCount(_deserializeFileWordsCount()),
      _tail(_deserializeTail()),
      _additionalBitsCnt(_computeAdditionalBitsCnt()) {}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
auto ArithmeticDecoder<SymT, DictT, CountT>::decode() -> Ret {
    typename ga::impl::CountTChoose<SymT>::Type value = 0;
    std::size_t valueBits = SymT::numBits + _additionalBitsCnt;

    for (auto _ : boost::irange<std::size_t>(0, valueBits)) {
        value = (value << 1) + (_source.takeBit() ? 1 : 0);
    }

    auto currRange = OrdRange { 0, correctedSymsNum };

    Ret ret;
    ret.tail = _tail;

    int lastPercent = -1;

    for (auto i : boost::irange<CountT>(0, _fileWordsCount)) {
        if (int currPercent = (100 * i) / _fileWordsCount;
                currPercent != lastPercent) {
            std::cerr << currPercent << '%' << std::endl;
            lastPercent = currPercent;
        }

        auto range = currRange.high - currRange.low;
        auto aux =
                ((value - currRange.low + 1) * _dict.totalWordsCount() - 1) / range;

        auto sym = _dict.getWord(aux);
        ret.syms.push_back(sym);

        auto h = _dict.getHigherCumulativeNumFound(sym);
        auto l = _dict.getLowerCumulativeNumFound(sym);

        currRange = OrdRange {
            currRange.low + (range * l) / _dict.totalWordsCount(),
            currRange.low + (range * h) / _dict.totalWordsCount()
        };

        if constexpr (dict::traits::needWordIncrease<DictT>) {
            _dict.increaseWordCount(sym);
        }

        while (true) {
            if (currRange.high <= correctedSymsNum_2) {
                bool bit = _source.takeBit();
                value = value * 2 + (bit ? 1 : 0);
            } else if (currRange.low >= correctedSymsNum_2) {
                bool bit = _source.takeBit();
                value = value * 2 - correctedSymsNum + (bit ? 1 : 0);
            } else if (currRange.low >= correctedSymsNum_4
                       && currRange.high <= correctedSymsNum_3to4) {
                bool bit = _source.takeBit();
                value = value * 2 - correctedSymsNum_2 + (bit ? 1 : 0);
            } else {
                break;
            }
            currRange = RangesCalc<SymT>::recalcRange(currRange);
        }
    }

    std::cerr << "100%" << std::endl;
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
auto ArithmeticDecoder<SymT, DictT, CountT>::_deserializeTail() -> Tail {
    std::uint16_t tailSize = _source.takeT<std::uint16_t>();
    Tail tail(tailSize);
    for (auto& tailBit: tail) {
        tailBit = _source.takeBit();
    }
    return tail;
}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
std::vector<std::uint64_t>
ArithmeticDecoder<SymT, DictT, CountT>::_deserializeWordsCounts() {
    std::vector<std::uint64_t> ret(SymT::wordsCount);
    CountT numUniqueSyms = _source.takeT<std::uint32_t>();

    for (auto _ : boost::irange<CountT>(0, numUniqueSyms)) {
        auto sym = _source.takeT<SymT>();
        auto numFound = _source.takeT<CountT>();
        ret[SymT::ord(sym)] = numFound;
    }

    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
CountT ArithmeticDecoder<SymT, DictT, CountT>::_deserializeFileWordsCount() {
    return _source.takeT<CountT>();
}

//----------------------------------------------------------------------------//
template <class SymT, class DictT, typename CountT>
std::uint8_t
ArithmeticDecoder<SymT, DictT, CountT>::_computeAdditionalBitsCnt() const {
    std::uint8_t ret = 0;
    for (; (symsNum << ret) < (1ull << 33); ++ret) {}
    return ret;
}

}  // namespace ga

#endif // ARITHMETIC_DECODER_HPP

