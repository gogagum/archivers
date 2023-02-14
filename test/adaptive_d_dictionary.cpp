#include <gtest/gtest.h>

#include <array>
#include <cstddef>

#include "dictionary/adaptive_d_dictionary.hpp"
#include "word/bytes_word.hpp"
#include "word/bits_word.hpp"

using ga::w::BytesWord;
using ga::w::BitsWord;
using ga::dict::AdaptiveDDictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, Construct) {
    auto dict = AdaptiveDDictionary<BytesWord<2>>();
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, GetStats) {
    auto dict = AdaptiveDDictionary<BytesWord<2>>();
    const auto wordData = std::array{ std::byte{0b00000000}, std::byte{0b00000001} };
    const auto word = BytesWord<2>(wordData.data());
    [[maybe_unused]] const auto _stats = dict.getProbabilityStats(word);
}

//----------------------------------------------------------------------------//
TEST(AdapriveDDictionary, GetStatsOnStartCenter) {
    auto dict = AdaptiveDDictionary<BitsWord<3>>();
    const auto testWordData = std::byte{ 0b110 };  // 6
    const auto word = BitsWord<3>(ga::impl::BitsIterator(testWordData) - 3);
    const auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 6);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, GetStatsOnStartEnd) {
    auto dict = AdaptiveDDictionary<BitsWord<3>>();
    const auto testWordData = std::byte{ 0b111 };  // 7
    const auto word = BitsWord<3>(ga::impl::BitsIterator(testWordData) - 3);
    const auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 7);
    EXPECT_EQ(high, 8);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, GetStatsOnStartBegin) {
    auto dict = AdaptiveDDictionary<BitsWord<3>>();
    const auto testWordData = std::byte{ 0b000 };  // 0
    const auto word = BitsWord<3>(ga::impl::BitsIterator(testWordData) - 3);
    const auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, Example) {
    auto dict = AdaptiveDDictionary<BytesWord<1>>();
    const auto getLetterStats = [&dict](char letter) {
        return dict.getProbabilityStats(BytesWord<1>::byOrd(letter));
    };
    const auto [low0, high0, total0] = getLetterStats('I');
    EXPECT_EQ(high0 - low0, 1);
    EXPECT_EQ(total0, 256);
    const auto [low1, high1, total1] = getLetterStats('F');
    EXPECT_EQ(high1 - low1, 1);
    EXPECT_EQ(total1, 255 * 2);
    const auto [low2, high2, total2] = getLetterStats('_');
    EXPECT_EQ(high2 - low2, 2);
    EXPECT_EQ(total2, 254 * 4);
    const auto [low3, high3, total3] = getLetterStats('W');
    EXPECT_EQ(high3 - low3, 3);
    EXPECT_EQ(total3, 253 * 6);
    const auto [low4, high4, total4] = getLetterStats('E');
    EXPECT_EQ(high4 - low4, 4);
    EXPECT_EQ(total4, 252 * 8);
    const auto [low5, high5, total5] = getLetterStats('_');
    EXPECT_EQ(high5 - low5, 251);
    EXPECT_EQ(total5, 251 * 10);
    const auto [low6, high6, total6] = getLetterStats('C');
    EXPECT_EQ(high6 - low6, 5);
    EXPECT_EQ(total6, 251 * 12);
    const auto [low7, high7, total7] = getLetterStats('A');
    EXPECT_EQ(high7 - low7, 6);
    EXPECT_EQ(total7, 250 * 14);
}
