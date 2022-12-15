#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/container/static_vector.hpp>
#include <boost/range/algorithm_ext/insert.hpp>
#include <boost/range/irange.hpp>
#include <map>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>

#include "arithmetic_decoder_decoded.hpp"
#include "ranges_calc.hpp"
#include "dictionary/static_dicitionary.hpp"

namespace ga {

namespace bc = boost::container;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class SymT, typename CountT = std::uint32_t>
class ArithmeticDecoder : RangesCalc<SymT> {
public:
    using Source = ArithmeticDecoderDecoded;
public:

    /**
     * @brief ArithmeticDecoder constructor from file source.
     * @param source
     */
    ArithmeticDecoder(Source&& source);

    /**
     * @brief decode - decode source as a vector of bytes.
     * @return
     */
    std::vector<std::byte> decode();

private:

    using RangesCalc<SymT>::symsNum;
    using RangesCalc<SymT>::correctedSymsNum;
    using RangesCalc<SymT>::correctedSymsNum_2;
    using RangesCalc<SymT>::correctedSymsNum_4;
    using RangesCalc<SymT>::correctedSymsNum_3to4;

private:

    bc::static_vector<std::byte, SymT::numBytes> _deserializeTail();

    std::vector<std::uint64_t> _deserializeDict();

    std::uint8_t _computeAdditionalBitsCnt() const;

private:
    Source _source;
    const boost::container::static_vector<std::byte, SymT::numBytes> _tail;
    const dict::StaticDictionary<SymT> _dict;
    const std::uint8_t _additionalBitsCnt;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
ArithmeticDecoder<SymT,  CountT>::ArithmeticDecoder(Source&& source)
    : _source(std::move(source)),
      _tail(_deserializeTail()),
      _dict(_deserializeDict()),
      _additionalBitsCnt(_computeAdditionalBitsCnt()) {}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
std::vector<std::byte> ArithmeticDecoder<SymT, CountT>::decode() {
    std::uint64_t value = 0;
    std::size_t valueBits = SymT::numBytes * 8 + _additionalBitsCnt;
    assert(valueBits < 64 && "`value must be placeble in 64 bits");

    for (auto _ : boost::irange<std::size_t>(0, valueBits)) {
        value = (value << 1) + (_source.takeBit() ? 1 : 0);
    }

    auto currRange = typename RangesCalc<SymT>::Range { 0, correctedSymsNum };

    std::vector<SymT> syms;

    for (std::uint64_t i = 0; i < _dict.totalWordsCount(); ++i) {
        std::uint64_t range = currRange.high - currRange.low;
        std::uint64_t aux = ((value - currRange.low + 1) * _dict.totalWordsCount() - 1) / range;

        auto sym = _dict.getWord(aux);
        syms.push_back(sym);

        auto h = _dict.getHigherCumulativeNumFound(sym);
        auto l = _dict.getLowerCumulativeNumFound(sym);

        currRange = typename RangesCalc<SymT>::Range {
            currRange.low + (range * l) / _dict.totalWordsCount(),
            currRange.low + (range * h) / _dict.totalWordsCount()
        };

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
    std::vector<std::byte> ret(syms.size() * SymT::numBytes);
    std::memcpy(ret.data(), syms.data(), syms.size() * SymT::numBytes);
    boost::range::insert(ret, ret.end(), _tail);
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
bc::static_vector<std::byte, SymT::numBytes>
ArithmeticDecoder<SymT, CountT>::_deserializeTail() {
    std::uint8_t tailSize = _source.takeT<std::uint8_t>();
    boost::container::static_vector<std::byte, SymT::numBytes> tail(tailSize);
    for (auto& tailByte: tail) {
        tailByte = _source.takeByte();
    }
    return tail;
}

template <class SymT, typename CountT>
std::vector<std::uint64_t> ArithmeticDecoder<SymT, CountT>::_deserializeDict() {
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
template <class SymT, typename CountT>
std::uint8_t
ArithmeticDecoder<SymT, CountT>::_computeAdditionalBitsCnt() const {
    std::uint8_t ret = 0;
    for (; (symsNum << ret) < (std::uint64_t{1} << 40); ++ret) {
    }
    return ret;
}

}  // ga

#endif // ARITHMETIC_DECODER_HPP

