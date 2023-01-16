#include <gtest/gtest.h>

#include "../include/arithmetic_coder.hpp"
#include "../include/flow/bits_word_flow.hpp"
#include "../include/dictionary/adaptive/adaptive_dictionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticCoder, LambdaAsConstruct) {
    using Word = ga::w::BitsWord<3>;
    using Dict = ga::dict::AdaptiveDictionary<Word>;
    auto dataBytes = std::array<std::byte, 32>{};
    auto coder = ga::ArithmeticCoder<ga::fl::BitsWordFlow<Word>, Dict>(
        ga::fl::BitsWordFlow<Word>(dataBytes),
        []() -> Dict { return Dict(12); }
    );
}

