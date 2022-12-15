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
#include "dictionary/static_dicitionary.hpp"

namespace ga {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class SymT, typename CountT = std::uint32_t>
class ArithmeticDecoder{
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
    ////////////////////////////////////////////////////////////////////////////
    /// \brief The SymInfo class
    ///
    struct SymInfo {
        SymT sym;
        CountT count;

        ////////////////////////////////////////////////////////////////////////
        /// \brief The SymOrder class
        ///
        struct SymOrder {
            bool operator()(const SymInfo& si1, const SymInfo& si2) const;
        };
    };

    constexpr static auto symsNum = static_cast<std::uint64_t>(SymT::wordsCount);
    constexpr static auto symsNum_2 = symsNum / 2;
    constexpr static auto symsNum_4 = symsNum / 4;
    constexpr static auto symsNum_3to4 = 3 * symsNum / 4;

private:

    boost::container::static_vector<std::byte, SymT::numBytes> _deserializeTail();

    std::vector<std::uint64_t> _deserializeDict();

    std::uint8_t _computeAdditionalBitsCnt() const;

private:
    Source _source;
    const boost::container::static_vector<std::byte, SymT::numBytes> _tail;
    const dict::StaticDictionary<SymT> _dict;
    const std::uint8_t _additionalBitsCnt;
    const std::uint64_t _correctingConst;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
ArithmeticDecoder<SymT,  CountT>::ArithmeticDecoder(Source&& source)
    : _source(std::move(source)),
      _tail(_deserializeTail()),
      _dict(_deserializeDict()),
      _additionalBitsCnt(_computeAdditionalBitsCnt()),
      _correctingConst(std::uint64_t{1} << _additionalBitsCnt) {}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
std::vector<std::byte> ArithmeticDecoder<SymT, CountT>::decode() {
    std::uint64_t value = 0;
    std::size_t valueBits = SymT::numBytes * 8 + _additionalBitsCnt;
    assert(valueBits < 64 && "`value must be placeble in 64 bits");

    for (auto _ : boost::irange<std::size_t>(0, valueBits)) {
        value = (value << 1) + (_source.takeBit() ? 1 : 0);
    }

    std::uint64_t low = 0;
    std::uint64_t high = symsNum * _correctingConst;

    std::vector<SymT> syms;

    for (std::uint64_t i = 0; i < _dict.totalWordsCount(); ++i) {
        std::uint64_t range = high - low;
        std::uint64_t aux = ((value - low + 1) * _dict.totalWordsCount() - 1) / range;

        auto sym = _dict.getWord(aux);
        syms.push_back(sym);

        high = low + (range * _dict.getHigherCumulativeNumFound(sym)) / _dict.totalWordsCount();
        low = low + (range * _dict.getLowerCumulativeNumFound(sym)) / _dict.totalWordsCount();

        while (true) {
            if (high <= symsNum_2 * _correctingConst) {
                high = high * 2;
                low = low * 2;
                bool bit = _source.takeBit();
                value = value * 2 + (bit ? 1 : 0);
            } else if (low >= symsNum_2 * _correctingConst) {
                high = high * 2 - symsNum * _correctingConst;
                low = low * 2 - symsNum * _correctingConst;
                bool bit = _source.takeBit();
                value = value * 2 - symsNum * _correctingConst + (bit ? 1 : 0);
            } else if (low >= symsNum_4 * _correctingConst
                       && high <= symsNum_3to4 * _correctingConst) {
                high = high * 2 - symsNum_2 * _correctingConst;
                low = low * 2 - symsNum_2 * _correctingConst;
                bool bit = _source.takeBit();
                value = value * 2 - symsNum_2 * _correctingConst + (bit ? 1 : 0);
            } else {
                break;
            }
        }
    }
    std::vector<std::byte> ret(syms.size() * SymT::numBytes);
    std::memcpy(ret.data(), syms.data(), syms.size() * SymT::numBytes);
    boost::range::insert(ret, ret.end(), _tail);
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
boost::container::static_vector<std::byte, SymT::numBytes>
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

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
bool ArithmeticDecoder<SymT, CountT>::SymInfo::SymOrder::operator() (
        const SymInfo& si1, const SymInfo& si2) const {
    const typename SymT::Order order;
    return order(si1.sym, si2.sym);
}

}  // ga

#endif // ARITHMETIC_DECODER_HPP

