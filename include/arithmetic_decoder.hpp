#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <boost/container/static_vector.hpp>
#include <map>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>

#include "arithmetic_decoder_decoded.hpp"
#include "misc.hpp"

namespace garchiever {

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

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The Alphabet class
    ///
    struct Alphabet {
        std::vector<SymInfo> cumulativeNumFound;
        std::uint64_t numUniqueSyms;
        CountT totalSymsCount;
    };

    constexpr static auto symsNum = static_cast<std::uint64_t>(SymT::maxNum);
    constexpr static auto symsNum_2 = symsNum / 2;
    constexpr static auto symsNum_4 = symsNum / 4;
    constexpr static auto symsNum_3to4 = 3 * symsNum / 4;

private:

    CountT _getCumulativeNumFoundLow(const SymT& sym);

    CountT _getCumulativeNumFoundHigh(const SymT& sym);

    boost::container::static_vector<std::byte, SymT::numBytes> _deserializeTail();

    Alphabet _deserializeAlphabet();

    std::uint8_t _computeAdditionalBitsCnt() const;

private:
    Source _source;
    const boost::container::static_vector<std::byte, SymT::numBytes> _tail;
    const Alphabet _alphabet;
    const std::uint8_t _additionalBitsCnt;
    const std::uint64_t _correctingConst;
};

}  // garchiever

#endif // ARITHMETIC_DECODER_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
garchiever::ArithmeticDecoder<SymT,  CountT>::ArithmeticDecoder(Source&& source)
    : _source(std::move(source)),
      _tail(_deserializeTail()),
      _alphabet(_deserializeAlphabet()),
      _additionalBitsCnt(_computeAdditionalBitsCnt()),
      _correctingConst(std::uint64_t{1} << _additionalBitsCnt) {}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
std::vector<std::byte>
garchiever::ArithmeticDecoder<SymT, CountT>::decode() {
    std::uint64_t value = 0;

    for (std::size_t bitIndex = 0;
         bitIndex < SymT::numBytes * 8 + _additionalBitsCnt;
         ++bitIndex) {
        value = (value << 1) + (_source.takeBit() ? 1 : 0);
    }

    std::uint64_t low = 0;
    std::uint64_t high = symsNum * _correctingConst;

    std::vector<SymT> syms;

    for (std::uint64_t i = 0; i < _alphabet.totalSymsCount; ++i) {
        std::uint64_t range = high - low;
        std::uint64_t aux = ((value - low + 1) * _alphabet.totalSymsCount - 1);

        auto it = _alphabet.cumulativeNumFound.begin();
        bool found = false;

        while (!found) {
            std::size_t offset = 1;
            while ((it + offset * 2 - 1) < _alphabet.cumulativeNumFound.end()
                   && (it + offset * 2 - 1)->count * range <= aux) {
                offset *= 2;
            }
            it += (offset - 1);
            found = (offset == 1);
        }

        auto sym = it->sym;
        syms.push_back(sym);

        high = low + (range * _getCumulativeNumFoundHigh(sym)) / _alphabet.totalSymsCount;
        low = low + (range * _getCumulativeNumFoundLow(sym)) / _alphabet.totalSymsCount;

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
            } else if (low >= symsNum_4 * _correctingConst && high <= symsNum_3to4 * _correctingConst) {
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
    ret.insert(ret.end(), _tail.begin(), _tail.end());
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
auto
garchiever::ArithmeticDecoder<SymT, CountT>::_getCumulativeNumFoundLow(
        const SymT &sym) -> CountT {
    auto fakeInfo = SymInfo(sym, 0);
    auto iter = std::lower_bound(_alphabet.cumulativeNumFound.begin(),
                                 _alphabet.cumulativeNumFound.end(),
                                 fakeInfo, typename SymInfo::SymOrder());
    assert(iter != _alphabet.cumulativeNumFound.end());

    return iter->count;
}


//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
auto
garchiever::ArithmeticDecoder<SymT, CountT>::_getCumulativeNumFoundHigh(
        const SymT &sym) -> CountT {
    auto fakeInfo = SymInfo(sym, 0);
    auto iter = std::lower_bound(_alphabet.cumulativeNumFound.begin(),
                                 _alphabet.cumulativeNumFound.end(),
                                 fakeInfo, typename SymInfo::SymOrder());
    assert(iter != _alphabet.cumulativeNumFound.end());
    if (auto nextIter = std::next(iter);
            nextIter == _alphabet.cumulativeNumFound.end()) {
        return _alphabet.totalSymsCount;
    } else {
        return nextIter->count;
    }
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
boost::container::static_vector<std::byte, SymT::numBytes>
garchiever::ArithmeticDecoder<SymT, CountT>::_deserializeTail() {
    std::uint8_t tailSize = _source.takeT<std::uint8_t>();
    boost::container::static_vector<std::byte, SymT::numBytes> tail(tailSize);
    for (auto& tailByte: tail) {
        tailByte = _source.takeByte();
    }
    return tail;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
auto garchiever::ArithmeticDecoder<SymT, CountT>::_deserializeAlphabet(
        ) -> Alphabet {
    Alphabet ret;
    ret.numUniqueSyms = _source.takeT<std::uint32_t>();
    for (std::uint64_t i = 0; i < ret.numUniqueSyms; ++i) {        
        auto sym = _source.takeT<SymT>();
        auto numFound = _source.takeT<CountT>();
        ret.cumulativeNumFound.emplace_back(sym, numFound);
    }
    ret.totalSymsCount = _source.takeT<CountT>();
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
std::uint8_t garchiever::ArithmeticDecoder<SymT, CountT>::_computeAdditionalBitsCnt(
        ) const {
    std::uint8_t ret = 0;
    for (; (symsNum << ret) < (std::uint64_t{1} << 40); ++ret) {
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
bool
garchiever::ArithmeticDecoder<SymT, CountT>::SymInfo::SymOrder::operator() (
        const SymInfo& si1, const SymInfo& si2) const {
    const typename SymT::Order order;
    return order(si1.sym, si2.sym);
}

