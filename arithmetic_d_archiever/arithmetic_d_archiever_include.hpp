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

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint8_t numBytes>
using BytesDict = ga::dict::AdaptiveDDictionary<BytesWord<numBytes>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using BitsDict = ga::dict::AdaptiveDDictionary<BitsWord<numBits>>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
struct TypeChoise {
    using Dict = BitsDict<numBits>;
    using Flow = ga::fl::BitsWordFlow<numBits>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits> requires (numBits % 8 == 0)
struct TypeChoise<numBits>{
    using Dict = BytesDict<numBits/8>;
    using Flow = ga::fl::BytesWordFlow<numBits/8>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Dict = typename TypeChoise<numBits>::Dict;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Flow = typename TypeChoise<numBits>::Flow;

#endif // ARITHMETIC_D_ARCHIEVER_INCLUDE_HPP
