#include <gtest/gtest.h>

#include "../include/word/bytes_word.hpp"
#include "../include/word/bits_word.hpp"
#include "../include/word/integer_word.hpp"
#include "../include/dictionary/adaptive_dictionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DynamicDictionary, ConstructBytesWord) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<2>>();
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, ConstructBitsWord) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BitsWord<2>>();
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, ConstructIntWord) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::IntegerWord<int, 0, 3>>();
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, LowerAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>();
    auto symData = std::array{std::byte{37}};
    auto word = ga::w::BytesWord<1>(symData);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 37);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, LowerZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesWord<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, HigherZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>();
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesWord<1>(symData);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, Increase) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>();
    auto increasedWordData = std::array{std::byte{12}};
    dict.increaseWordCount(ga::w::BytesWord<1>(increasedWordData));
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesWord<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, IncreaseExactTheSame) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>();
    auto increasedWordData = std::array{std::byte{12}};
    auto increasedWord = ga::w::BytesWord<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 12);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 14);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, IncreaseZero) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>();
    auto increasedWordData = std::array{std::byte{0}};
    auto increasedWord = ga::w::BytesWord<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 0);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 2);
}
