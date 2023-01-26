#ifndef ARITHMETIC_ARCHIEVER_INCLUDE_HPP
#define ARITHMETIC_ARCHIEVER_INCLUDE_HPP

#include "arithmetic_coder.hpp"
#include "arithmetic_decoder.hpp"
#include "word/bytes_word.hpp"
#include "flow/bytes_word_flow.hpp"
#include "word/bits_word.hpp"
#include "flow/bits_word_flow.hpp"
#include "dictionary/adaptive_dictionary.hpp"

using ga::w::BytesWord;
using ga::w::BitsWord;
using ga::ArithmeticCoder;
using ga::ArithmeticDecoder;

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesFlow = ga::fl::BytesWordFlow<numBytes>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesDict = ga::dict::AdaptiveDictionary<BytesWord<numBytes>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesCoder = ArithmeticCoder<BytesFlow<numBytes>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsFlow = ga::fl::BitsWordFlow<numBits>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsDict = ga::dict::AdaptiveDictionary<BitsWord<numBits>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsCoder = ArithmeticCoder<BitsFlow<numBits>>;

#endif // ARITHMETIC_ARCHIEVER_INCLUDE_HPP
