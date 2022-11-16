#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <map>
#include <cstdint>
#include <cstring>

#include "arithmetic_decoder_decoded.hpp"

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticDecoder class
///
template <class SymT>
class ArithmeticDecoder{
public:
    using Source = ArithmeticDecoderDecoded;
public:
    ArithmeticDecoder(Source&& source);

    std::vector<std::byte> decode();

private:
    using MapToUint64T = std::map<SymT, std::uint64_t, typename SymT::Order>;

private:

    std::uint64_t _getCumulativeNumFoundLow(const SymT& sym);

    std::uint64_t _getCumulativeNumFoundHigh(const SymT& sym);

private:
    MapToUint64T _cumulativeNumFound;
    Source _source;
    std::uint64_t _numUniqueSyms;
    std::uint64_t _totalSymsCount;
};

}  // garchiever

#endif // ARITHMETIC_DECODER_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT>
garchiever::ArithmeticDecoder<SymT>::ArithmeticDecoder(Source&& source)
    : _source(std::move(source)),
      _numUniqueSyms{_source.takeT<std::uint64_t>()} {
    for (std::uint64_t i = 0; i < _numUniqueSyms; ++i) {
        auto sym = _source.takeT<SymT>();
        auto numFound = _source.takeT<std::uint64_t>();
        _cumulativeNumFound[sym] = numFound;
    }
    _totalSymsCount = _source.takeT<std::uint64_t>();
}

//----------------------------------------------------------------------------//
template <class SymT>
std::vector<std::byte>
garchiever::ArithmeticDecoder<SymT>::decode() {
    constexpr auto wordsNum = static_cast<std::uint64_t>(SymT::maxNum);
    constexpr auto wordsNum_2 = wordsNum / 2;
    constexpr auto wordsNum_4 = wordsNum / 4;
    constexpr auto wordsNum_3to4 = 3 * wordsNum / 4;


    std::uint64_t value = _source.takeT<std::uint64_t>();

    std::uint64_t low = 0;
    std::uint64_t high = _totalSymsCount;

    std::vector<SymT> syms;

    for (std::uint64_t i = 0; i < _totalSymsCount; ++i) {
        std::uint64_t range = high - low;
        std::uint64_t aux = (value - low) * _totalSymsCount / range;
        auto sumIter = _cumulativeNumFound.begin();
        while (sumIter->second < aux) {
            sumIter = std::next(sumIter);
        }
        auto sym = sumIter->first;
        syms.push_back(sym);
        high = low + range * _getCumulativeNumFoundHigh(sym) / _totalSymsCount;
        low = low + range * _getCumulativeNumFoundLow(sym) / _totalSymsCount;

        constexpr std::uint64_t one = std::uint64_t{1};
        constexpr std::uint64_t zero = std::uint64_t{0};

        while (true) {
            if (high <= wordsNum_2) {
                high = high * 2;
                low = low * 2;
                bool bit = _source.takeBit();
                value = value * 2 + (bit ? one : zero);
            } else if (low >= wordsNum_2) {
                high = high * 2 - wordsNum;
                low = low * 2 - wordsNum;
                bool bit = _source.takeBit();
                value = value * 2 - wordsNum + (bit ? one : zero);
            } else if (low >= wordsNum_4 && high <= wordsNum_3to4) {
                high = high * 2 - wordsNum_2;
                low = low * 2 - wordsNum_2;
                bool bit = _source.takeBit();
                value = value * 2 - wordsNum_2 + (bit ? one : zero);
            } else {
                break;
            }
        }
    }
    std::vector<std::byte> ret(syms.size() * SymT::numBytes);
    std::memcpy(ret.data(), syms.data(), syms.size() * SymT::numBytes);
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT>
std::uint64_t
garchiever::ArithmeticDecoder<SymT>::_getCumulativeNumFoundLow(const SymT &word) {
    assert(_cumulativeNumFound.contains(word));
    return _cumulativeNumFound[word];
}


//----------------------------------------------------------------------------//
template <class SymT>
std::uint64_t
garchiever::ArithmeticDecoder<SymT>::_getCumulativeNumFoundHigh(const SymT &word) {
    assert(_cumulativeNumFound.contains(word));
    auto iter = _cumulativeNumFound.find(word);
    if (auto nextIter = std::next(iter); nextIter == _cumulativeNumFound.end()) {
        return _totalSymsCount;
    } else {
        return nextIter->second;
    }
}
