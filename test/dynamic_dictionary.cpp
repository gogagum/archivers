#include <gtest/gtest.h>

#include "../include/word/bytes_symbol.hpp"
#include "../include/dictionary/adaptive_dictionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DynamicDictionary, Construct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<2>>();
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, LowerAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{37}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 37);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, LowerZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, HigherZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, Increase) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto increasedWordData = std::array{std::byte{12}};
    dict.increaseWordCount(ga::w::BytesSymbol<1>(increasedWordData));
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, IncreaseExactTheSame) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto increasedWordData = std::array{std::byte{12}};
    auto increasedWord = ga::w::BytesSymbol<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 12);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 14);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, IncreaseZero) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto increasedWordData = std::array{std::byte{0}};
    auto increasedWord = ga::w::BytesSymbol<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 0);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 2);
}
