#include <vector>

#include "flow/traits.hpp"
#include "flow/bytes_word_flow.hpp"
#include "flow/bits_word_flow.hpp"

struct Word;

using WordVec = std::vector<Word>;

using ga::fl::BytesWordFlow;
using ga::fl::BitsWordFlow;
using ga::w::BytesWord;
using ga::w::BitsWord;

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(std::is_same_v<typename ga::fl::traits::WordT<WordVec>, Word>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(std::is_same_v<typename ga::fl::traits::WordT<const WordVec>, Word>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(std::is_same_v<
    typename ga::fl::traits::WordT<BytesWordFlow<3>>, BytesWord<3>
>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(std::is_same_v<
    typename ga::fl::traits::WordT<BitsWordFlow<45>>, BitsWord<45>
>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(std::is_same_v<
    typename ga::fl::traits::WordT<const BytesWordFlow<3>>, BytesWord<3>
>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(std::is_same_v<
    typename ga::fl::traits::WordT<const BitsWordFlow<45>>, BitsWord<45>
>);
