#pragma once

#ifndef ARITHMETIC_DECODER_HPP
#define ARITHMETIC_DECODER_HPP

#include <map>
#include <cstdint>
#include <cstring>

#include "arithmetic_decoder_decoded.hpp"
#include "misc.hpp"

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
    std::vector<std::byte> _tail;
};

}  // garchiever

#endif // ARITHMETIC_DECODER_HPP

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class SymT>
garchiever::ArithmeticDecoder<SymT>::ArithmeticDecoder(Source&& source)
    : _source(std::move(source)) {
    std::uint8_t tailSize = _source.takeT<std::uint8_t>();
    _tail.resize(tailSize);
    std::cerr << "Tail size: " << static_cast<unsigned int>(tailSize) << std::endl;
    for (auto& tailByte: _tail) {
        tailByte = _source.takeByte();
        std::cerr << tailByte << ' ';
    }
    std::cerr << std::endl;
    _numUniqueSyms = _source.takeT<std::uint64_t>();
    std::cerr << "Unique syms count: " << _numUniqueSyms << std::endl;
    for (std::uint64_t i = 0; i < _numUniqueSyms; ++i) {
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
        auto numFound = _source.takeT<std::uint64_t>();
        std::cerr << " Cumulative num: " << numFound << std::endl;
        _cumulativeNumFound[sym] = numFound;
    }
    _totalSymsCount = _source.takeT<std::uint64_t>();
    std::cerr << "Total count: " << _totalSymsCount << std::endl;
}

//----------------------------------------------------------------------------//
template <class SymT>
std::vector<std::byte>
garchiever::ArithmeticDecoder<SymT>::decode() {
    constexpr auto wordsNum = static_cast<std::uint64_t>(SymT::maxNum);
    constexpr auto wordsNum_2 = wordsNum / 2;
    constexpr auto wordsNum_4 = wordsNum / 4;
    constexpr auto wordsNum_3to4 = 3 * wordsNum / 4;

    std::uint64_t value = 0;

    for (std::size_t bitIndex = 0; bitIndex < SymT::numBytes * 8; ++bitIndex) {
        std::uint64_t digit = _source.takeBit() ? std::uint64_t{1} : std::uint64_t{0};
        value *= 2;
        value += digit;
    }

    std::uint64_t low = 0;
    std::uint64_t high = wordsNum;

    std::vector<SymT> syms;

    for (std::uint64_t i = 0; i < _totalSymsCount; ++i) {
        //assert(value <= high);
        std::uint64_t range = high - low;
        std::int64_t aux = ((value - low) * _totalSymsCount);

        auto symFound = SymT();
        bool found = false;

        for (auto [sym, _]: _cumulativeNumFound) {
            if (_getCumulativeNumFoundHigh(sym) * range >= aux
                && _getCumulativeNumFoundLow(sym) * range <= aux) {
                symFound = sym;
                found = true;
                break;
            }
        }

        if (!found) {
            symFound = _cumulativeNumFound.rbegin()->first;
        }

        syms.push_back(symFound);

        high = low + range * _getCumulativeNumFoundHigh(symFound) / _totalSymsCount;
        low = low + range * _getCumulativeNumFoundLow(symFound) / _totalSymsCount;

        constexpr std::uint64_t one = std::uint64_t{1};
        constexpr std::uint64_t zero = std::uint64_t{0};

        while (true) {
            if (high <= wordsNum_2) {
                high = high * 2;
                low = low * 2;
                bool bit = _source.takeBit();
                value = value * 2 + (bit ? one : zero);
                value %= SymT::maxNum;
            } else if (low >= wordsNum_2) {
                high = high * 2 - wordsNum;
                low = low * 2 - wordsNum;
                bool bit = _source.takeBit();
                value = value * 2 - wordsNum + (bit ? one : zero);
                value %= SymT::maxNum;
            } else if (low >= wordsNum_4 && high <= wordsNum_3to4) {
                high = high * 2 - wordsNum_2;
                low = low * 2 - wordsNum_2;
                bool bit = _source.takeBit();
                value = value * 2 - wordsNum_2 + (bit ? one : zero);
                value %= SymT::maxNum;
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
