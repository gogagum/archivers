#ifndef APPLIB_WORDS_AND_FLOW_HPP
#define APPLIB_WORDS_AND_FLOW_HPP

#include <cstdint>

#include <ael/word/bytes_word.hpp>
#include <ael/word/bits_word.hpp>
#include <ael/flow/bits_word_flow.hpp>
#include <ael/flow/bytes_word_flow.hpp>

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
struct TypeChoise {
    using Flow = ael::fl::BitsWordFlow<numBits>;
    using Word = ael::w::BitsWord<numBits>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits> requires (numBits % 8 == 0)
struct TypeChoise<numBits>{
    using Flow = ael::fl::BytesWordFlow<numBits/8>;
    using Word = ael::w::BytesWord<numBits/8>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Flow = typename TypeChoise<numBits>::Flow;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Word = typename TypeChoise<numBits>::Word;

#endif
