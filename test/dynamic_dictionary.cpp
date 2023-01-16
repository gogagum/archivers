#include <gtest/gtest.h>

#include "../include/word/bytes_word.hpp"
#include "../include/word/bits_word.hpp"
#include "../include/word/integer_word.hpp"
#include "../include/dictionary/adaptive_dictionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructBytesWord) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<2>>(1);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructBitsWord) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BitsWord<2>>(2);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructIntWord) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::IntegerWord<int, 0, 3>>(3);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(3);
    auto symData = std::array{std::byte{37}};
    auto word = ga::w::BytesWord<1>(symData);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 37);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(2);
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesWord<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, HigherZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(2);
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesWord<1>(symData);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, Increase) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(1);
    auto increasedWordData = std::array{std::byte{12}};
    dict.increaseWordCount(ga::w::BytesWord<1>(increasedWordData));
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesWord<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseExactTheSame) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(1);
    auto increasedWordData = std::array{std::byte{12}};
    auto increasedWord = ga::w::BytesWord<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 12);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseZero) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(1);
    auto increasedWordData = std::array{std::byte{0}};
    auto increasedWord = ga::w::BytesWord<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 0);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 2);
}
