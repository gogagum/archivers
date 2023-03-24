#ifndef APPLIB_WORDS_AND_FLOW_HPP
#define APPLIB_WORDS_AND_FLOW_HPP

#include <cstdint>

#include <applib/word/bytes_word.hpp>
#include <applib/word/bits_word.hpp>
#include <applib/flow/bits_word_flow.hpp>
#include <applib/flow/bytes_word_flow.hpp>

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
struct TypeChoise {
    using Flow = BitsWordFlow<numBits>;
    using Word = BitsWord<numBits>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits> requires (numBits % 8 == 0)
struct TypeChoise<numBits>{
    using Flow = BytesWordFlow<numBits/8>;
    using Word = BytesWord<numBits/8>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Flow = typename TypeChoise<numBits>::Flow;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t numBits>
using Word = typename TypeChoise<numBits>::Word;

#endif  // APPLIB_WORDS_AND_FLOW_HPP
