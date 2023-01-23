#ifndef ARITHMETIC_D_ARCHIEVER_INCLUDE_HPP
#define ARITHMETIC_D_ARCHIEVER_INCLUDE_HPP

#include "arithmetic_coder.hpp"
#include "arithmetic_decoder.hpp"
#include "word/bytes_word.hpp"
#include "flow/bytes_word_flow.hpp"
#include "word/bits_word.hpp"
#include "flow/bits_word_flow.hpp"
#include "dictionary/adaptive_d_dictionary.hpp"

using ga::w::BytesWord;
using ga::w::BitsWord;
using ga::ArithmeticCoder;
using ga::ArithmeticDecoder;

constexpr static const std::uint16_t rangeNumBits = 60;

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesFlow = ga::fl::BytesWordFlow<BytesWord<numBytes>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesDict = ga::dict::AdaptiveDDictionary<BytesWord<numBytes>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesCoder = ArithmeticCoder<BytesFlow<numBytes>, BytesDict<numBytes>, rangeNumBits>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesDecoder = ArithmeticDecoder<BytesWord<numBytes>, BytesDict<numBytes>, rangeNumBits>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsFlow = ga::fl::BitsWordFlow<BitsWord<numBits>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsDict = ga::dict::AdaptiveDDictionary<BitsWord<numBits>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsCoder = ArithmeticCoder<BitsFlow<numBits>, BitsDict<numBits>, rangeNumBits>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsDecoder = ArithmeticDecoder<BitsWord<numBits>, BitsDict<numBits>, rangeNumBits>;

#endif // ARITHMETIC_D_ARCHIEVER_INCLUDE_HPP
