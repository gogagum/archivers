#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <map>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iostream>

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
    ArithmeticDecoder(Source&& source);

    std::vector<std::byte> decode();

private:
    using MapSymToCount = std::map<SymT, CountT, typename SymT::Order>;

    struct Alphabet {
        MapSymToCount cumulativeNumFound;
        std::uint64_t numUniqueSyms;
        CountT totalSymsCount;
    };

    constexpr static auto wordsNum = static_cast<std::uint64_t>(SymT::maxNum);
    constexpr static auto wordsNum_2 = wordsNum / 2;
    constexpr static auto wordsNum_4 = wordsNum / 4;
    constexpr static auto wordsNum_3to4 = 3 * wordsNum / 4;

private:

    CountT _getCumulativeNumFoundLow(const SymT& sym);

    CountT _getCumulativeNumFoundHigh(const SymT& sym);

    std::vector<std::byte> _deserializeTail();

    Alphabet _deserializeAlphabet();

private:
    Source _source;
    std::vector<std::byte> _tail;
    Alphabet _alphabet;
};

}  // garchiever

#endif // ARITHMETIC_DECODER_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
garchiever::ArithmeticDecoder<SymT,  CountT>::ArithmeticDecoder(Source&& source)
    : _source(std::move(source)),
      _tail(_deserializeTail()),
      _alphabet(_deserializeAlphabet()) { }

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
std::vector<std::byte>
garchiever::ArithmeticDecoder<SymT, CountT>::decode() {
    std::uint64_t value = 0;

    for (std::size_t bitIndex = 0; bitIndex < SymT::numBytes * 8; ++bitIndex) {
        std::uint64_t digit = _source.takeBit() ? std::uint64_t{1} : std::uint64_t{0};
        value *= 2;
        value += digit;
    }

    std::uint64_t low = 0;
    std::uint64_t high = wordsNum - 1;

    std::vector<SymT> syms;

    for (std::uint64_t i = 0; i < _alphabet.totalSymsCount; ++i) {
        std::uint64_t range = high - low + 1;
        std::uint64_t aux = ((value - low + 1) * _alphabet.totalSymsCount);

        std::optional<SymT> symOpt;

        for (auto [sym, _]: _alphabet.cumulativeNumFound) {
            if (_getCumulativeNumFoundHigh(sym) * range > aux) {
                symOpt = sym;
                break;
            }
        }

        assert(symOpt.has_value());
        auto sym = symOpt.value();
        syms.push_back(sym);

        high = low + (range * _getCumulativeNumFoundHigh(sym)) / _alphabet.totalSymsCount - 1;
        low = low + (range * _getCumulativeNumFoundLow(sym)) / _alphabet.totalSymsCount;

        constexpr std::uint64_t one = std::uint64_t{1};
        constexpr std::uint64_t zero = std::uint64_t{0};

        while (true) {
            if (high < wordsNum_2) {
                high = high * 2 + 1;
                low = low * 2;
                bool bit = _source.takeBit();
                value = value * 2 + (bit ? one : zero);
            } else if (low >= wordsNum_2) {
                high = high * 2 - wordsNum + 1;
                low = low * 2 - wordsNum;
                bool bit = _source.takeBit();
                value = value * 2 - wordsNum + (bit ? one : zero);
            } else if (low >= wordsNum_4 && high < wordsNum_3to4) {
                high = high * 2 - wordsNum_2 + 1;
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
    ret.insert(ret.end(), _tail.begin(), _tail.end());
    return ret;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
auto
garchiever::ArithmeticDecoder<SymT, CountT>::_getCumulativeNumFoundLow(
        const SymT &word) -> CountT {
    assert(_alphabet.cumulativeNumFound.contains(word));
    return _alphabet.cumulativeNumFound[word];
}


//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
auto
garchiever::ArithmeticDecoder<SymT, CountT>::_getCumulativeNumFoundHigh(
        const SymT &word) -> CountT {
    assert(_alphabet.cumulativeNumFound.contains(word));
    auto iter = _alphabet.cumulativeNumFound.find(word);
    if (auto nextIter = std::next(iter);
            nextIter == _alphabet.cumulativeNumFound.end()) {
        return _alphabet.totalSymsCount;
    } else {
        return nextIter->second;
    }
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
std::vector<std::byte>
garchiever::ArithmeticDecoder<SymT, CountT>::_deserializeTail() {
    std::uint8_t tailSize = _source.takeT<std::uint8_t>();
    std::vector<std::byte> tail(tailSize);
    std::cerr << "Tail size: " << static_cast<unsigned int>(tailSize) << std::endl;
    for (auto& tailByte: _tail) {
        tailByte = _source.takeByte();
        std::cerr << tailByte << ' ';
    }
    std::cerr << std::endl;
    return tail;
}

//----------------------------------------------------------------------------//
template <class SymT, typename CountT>
auto garchiever::ArithmeticDecoder<SymT, CountT>::_deserializeAlphabet(
        ) -> Alphabet {
    Alphabet ret;
    ret.numUniqueSyms = _source.takeT<std::uint32_t>();
    std::cerr << "Unique syms count: " << ret.numUniqueSyms << std::endl;
    for (std::uint64_t i = 0; i < ret.numUniqueSyms; ++i) {
        auto sym = _source.takeT<SymT>();
        std::cerr << "Sym: " << sym;
        if constexpr (SymT::numBytes == 1) {
            std::cerr << " SymChar: ";
            if (char& asChar = *reinterpret_cast<char*>(&sym); asChar == '\0') {
                std::cerr << "\\0";
            } else if (asChar == '\n') {
                std::cerr << "\\n";
            } else {
                std::cerr << asChar;
            }
        }
        auto numFound = _source.takeT<CountT>();
        std::cerr << " Cumulative num: " << numFound << std::endl;
        ret.cumulativeNumFound[sym] = numFound;
    }
    ret.totalSymsCount = _source.takeT<CountT>();
    std::cerr << "Total count: " << ret.totalSymsCount << std::endl;
    return ret;
}

