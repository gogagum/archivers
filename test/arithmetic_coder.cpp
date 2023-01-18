#include <gtest/gtest.h>

#include "../include/arithmetic_coder.hpp"
#include "../include/flow/bits_word_flow.hpp"
#include "../include/dictionary/adaptive_dictionary.hpp"
#include "../include/dictionary/adaptive_a_dictionary.hpp"
#include "../include/dictionary/static_dictionary.hpp"

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
TEST(ArithmeticCoder, LambdaConstruct1) {
    using Word = ga::w::BitsWord<3>;
    using Dict = ga::dict::StaticDictionary<Word>;
    auto dataBytes = std::array<std::byte, 32>{};
    auto coder = ga::ArithmeticCoder<ga::fl::BitsWordFlow<Word>, Dict>(
        ga::fl::BitsWordFlow<Word>(dataBytes),
        []() -> Dict { return Dict::fromCounts(std::initializer_list<std::uint64_t>{2, 4, 6, 7}); }
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

