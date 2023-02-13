#include <gtest/gtest.h>

#include <array>
#include <cstddef>

#include "dictionary/uniform_dictionary.hpp"
#include "word/bytes_word.hpp"

using ga::dict::UniformDictionary;
using ga::w::BytesWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(UniformDictionary, Construct) {
    const auto dict = UniformDictionary<BytesWord<2>>();
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, Ord2Bytes) {
    const auto dict = UniformDictionary<BytesWord<2>>();
    EXPECT_EQ(dict.getWord(2), BytesWord<2>::byOrd(2));
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, Ord3Bytes) {
    const auto dict = UniformDictionary<BytesWord<3>>();
    EXPECT_EQ(dict.getWord(26), BytesWord<3>::byOrd(26));
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, OrdLongOrd) {
    const auto dict = UniformDictionary<BytesWord<2>>();
    const auto symBytes = std::array{ std::byte{8}, std::byte{42} };
    EXPECT_EQ(dict.getWord(256 * 8 + 42), BytesWord<2>(symBytes.begin()));
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundLow) {
    auto dict = UniformDictionary<BytesWord<1>>();
    const auto word = BytesWord<1>::byOrd(37);
    const auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 37);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundLowZero) {
    auto dict = UniformDictionary<BytesWord<1>>();
    const auto word = BytesWord<1>::byOrd(0);
    const auto [low, _0, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundHigh) {
    auto dict = UniformDictionary<BytesWord<1>>();
    const auto word = BytesWord<1>::byOrd(37);
    const auto [_0, high, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(high, 38);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, CumulativeNumFoundHighZero) {
    auto dict = UniformDictionary<BytesWord<1>>();
    const auto word = BytesWord<1>::byOrd(0);
    const auto [_0, high, _1] = dict.getProbabilityStats(word);
    EXPECT_EQ(high, 1);
}

//----------------------------------------------------------------------------//
TEST(UniformDictionary, TotalWordsCount) {
    auto dict = UniformDictionary<BytesWord<1>>();
    const auto word = BytesWord<1>::byOrd(0);
    const auto [_0, _1, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(total, 256);
}
