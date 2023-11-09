#ifndef APPLIB_WORDS_AND_FLOW_HPP
#define APPLIB_WORDS_AND_FLOW_HPP

#include <applib/flow/bits_word_flow.hpp>
#include <applib/flow/bytes_word_flow.hpp>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t NumBits>
struct TypeChoice {
  using Flow = BitsWordFlow<NumBits>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <std::uint16_t NumBits>
  requires(NumBits % CHAR_BIT == 0)
struct TypeChoice<NumBits> {
  using Flow = BytesWordFlow<NumBits / CHAR_BIT>;
};

#endif  // APPLIB_WORDS_AND_FLOW_HPP
