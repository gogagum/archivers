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
    auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 37);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(2);
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesWord<1>(symData.data());
    auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, HigherZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(2);
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesWord<1>(symData);
    auto [_0, high, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(high, 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, Increase) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(1);
    auto increasedWordData = std::array{std::byte{12}};
    auto stats = dict.getProbabilityStats(ga::w::BytesWord<1>(increasedWordData));
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesWord<1>(symData.data());
    auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseExactTheSame) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(1);
    auto increasedWordData = std::array{std::byte{12}};
    auto increasedWord = ga::w::BytesWord<1>(increasedWordData);
    auto stats = dict.getProbabilityStats(increasedWord);  // TODO
    auto [low, high, _] = dict.getProbabilityStats(increasedWord);
    EXPECT_EQ(low, 12);
    EXPECT_EQ(high, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseZero) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<1>>(1);
    auto increasedWordData = std::array{std::byte{0}};
    auto increasedWord = ga::w::BytesWord<1>(increasedWordData);
    auto stats = dict.getProbabilityStats(increasedWord);  // TODO
    auto [low, high, _] = dict.getProbabilityStats(increasedWord);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 2);
}
