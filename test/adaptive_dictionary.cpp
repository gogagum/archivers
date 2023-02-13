#include <gtest/gtest.h>

#include "word/bytes_word.hpp"
#include "word/bits_word.hpp"
#include "word/integer_word.hpp"
#include "dictionary/adaptive_dictionary.hpp"
#include "byte_data_constructor.hpp"

using ga::dict::AdaptiveDictionary;
using ga::w::BytesWord;
using ga::w::BitsWord;
using ga::w::IntegerWord;
using ga::ByteDataConstructor;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructBytesWord) {
    auto dict = AdaptiveDictionary<BytesWord<2>>(1);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructBitsWord) {
    auto dict = AdaptiveDictionary<BitsWord<2>>(2);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, ConstructIntWord) {
    auto dict = AdaptiveDictionary<IntegerWord<int, 0, 3>>(3);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerAfterConstruct) {
    auto dict = AdaptiveDictionary<ga::w::BytesWord<1>>(3);
    const auto word = BytesWord<1>(std::array{std::byte{37}}.begin());
    auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 37);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, LowerZeroAfterConstruct) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(2);
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = BytesWord<1>(symData.data());
    auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, HigherZeroAfterConstruct) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(2);
    const auto word = ga::w::BytesWord<1>(std::array{std::byte{42}}.begin());
    auto [_0, high, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(high, 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, Increase) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(1);
    [[maybe_unused]] const auto stats =
        dict.getProbabilityStats(
            BytesWord<1>(std::array{std::byte{12}}.begin()));
    auto symData = std::array{std::byte{42}};
    auto word = BytesWord<1>(symData.data());
    auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 43);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseExactTheSame) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(1);
    const auto increasedWord = BytesWord<1>(std::array{std::byte{12}}.begin());

    auto [low1, high1, _1] = dict.getProbabilityStats(increasedWord);
    EXPECT_EQ(low1, 12);  // before first increase
    EXPECT_EQ(high1, 13); // before first increase
    auto [low2, high2, _2] = dict.getProbabilityStats(increasedWord);
    EXPECT_EQ(low2, 12);  // after first increase
    EXPECT_EQ(high2, 14); // after first increase
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, IncreaseZero) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(1);
    const auto increasedWord = BytesWord<1>(std::array{std::byte{0}}.begin());

    auto [low1, high1, _1] = dict.getProbabilityStats(increasedWord);
    EXPECT_EQ(low1, 0);   // before first increase
    EXPECT_EQ(high1, 1);  // before first increase
    auto [low2, high2, _2] = dict.getProbabilityStats(increasedWord);
    EXPECT_EQ(low2, 0);   // after first increase
    EXPECT_EQ(high2, 2);  // after first increase
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, GetWordOnStart) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(1);
    auto word = dict.getWord(42);
    EXPECT_EQ(word, BytesWord<1>::byOrd(42));
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, GetWordAfterUpdate) {
    auto dict = AdaptiveDictionary<BytesWord<1>>(1);
    [[maybe_unused]] auto stats =
            dict.getProbabilityStats(BytesWord<1>::byOrd(42));
    EXPECT_EQ(dict.getWord(42), BytesWord<1>::byOrd(42));
    EXPECT_EQ(dict.getWord(43), BytesWord<1>::byOrd(42));
}
