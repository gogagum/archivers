#ifndef ARITHMETIC_A_ARCHIEVER_INCLUDE_HPP
#define ARITHMETIC_A_ARCHIEVER_INCLUDE_HPP

#include "arithmetic_coder.hpp"
#include "arithmetic_decoder.hpp"
#include "word/bytes_word.hpp"
#include "flow/bytes_word_flow.hpp"
#include "word/bits_word.hpp"
#include "flow/bits_word_flow.hpp"
#include "dictionary/adaptive_a_dictionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
struct TypeChoise {
    using Dict = ga::dict::AdaptiveADictionary<ga::w::BitsWord<numBits>>;
    using Flow = ga::fl::BitsWordFlow<numBits>;
    using WordVec = std::vector<ga::w::BitsWord<numBits>>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits> requires (numBits % 8 == 0)
struct TypeChoise<numBits>{
    using Dict = ga::dict::AdaptiveADictionary<ga::w::BytesWord<numBits/8>>;
    using Flow = ga::fl::BytesWordFlow<numBits/8>;
    using WordVec = std::vector<ga::w::BytesWord<numBits/8>>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Dict = typename TypeChoise<numBits>::Dict;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Flow = typename TypeChoise<numBits>::Flow;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using WordVec = typename TypeChoise<numBits>::WordVec;

#endif // ARITHMETIC_A_ARCHIEVER_INCLUDE_HPP
