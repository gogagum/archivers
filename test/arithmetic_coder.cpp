#include <gtest/gtest.h>

#include "arithmetic_coder.hpp"
#include "flow/bits_word_flow.hpp"
#include "dictionary/adaptive_dictionary.hpp"
#include "dictionary/adaptive_a_dictionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticCoder, LambdaConstruct0) {
    using Word = ga::w::BitsWord<3>;
    using Dict = ga::dict::AdaptiveDictionary<Word>;
    auto dataBytes = std::array<std::byte, 32>{};
    auto coder = ga::ArithmeticCoder<ga::fl::BitsWordFlow<Word>, Dict>(
        ga::fl::BitsWordFlow<Word>(dataBytes),
        []() -> Dict { return Dict(12); }
    );
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoder, LambdaConstruct2) {
    using Word = ga::w::BitsWord<31>;
    using Dict = ga::dict::AdaptiveADictionary<Word>;
    auto dataBytes = std::array<std::byte, 32>{};
    auto coder = ga::ArithmeticCoder<ga::fl::BitsWordFlow<Word>, Dict>(
        ga::fl::BitsWordFlow<Word>(dataBytes),
        []() -> Dict { return Dict(); }
    );
}

