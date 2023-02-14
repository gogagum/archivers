#include <gtest/gtest.h>

#include <array>
#include <cstddef>

#include "dictionary/adaptive_a_dictionary.hpp"
#include "word/bytes_word.hpp"
#include "word/bits_word.hpp"

using ga::w::BytesWord;
using ga::w::BitsWord;
using ga::dict::AdaptiveADictionary;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, Construct) {
    auto dict = AdaptiveADictionary<BytesWord<2>>();
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStats) {
    auto dict = AdaptiveADictionary<BytesWord<2>>();
    [[maybe_unused]] const auto [low, high, total] =
            dict.getProbabilityStats(BytesWord<2>::byOrd(1));
}

//----------------------------------------------------------------------------//
TEST(AdapriveADictionary, GetStatsOnStartCenter) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto word = BitsWord<3>::byOrd(6);
    const auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 6);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsOnStartEnd) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto word = BitsWord<3>::byOrd(7);
    const auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 7);
    EXPECT_EQ(high, 8);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsOnStartBegin) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto word = BitsWord<3>::byOrd(0);
    const auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterUpdate) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto word = BitsWord<3>::byOrd(0);
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(word);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 14);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterCenter) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto updatedWord = BitsWord<3>::byOrd(2);
    const auto checkedWord = BitsWord<3>::byOrd(5);
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(updatedWord);
    const auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}


//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterBegin) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto updatedWordData = std::byte{ 0b01000000 };  // 2
    const auto checkedWordData = std::byte{ 0b00000000 };  // 0
    const auto updatedWord = BitsWord<3>::byOrd(2);
    const auto checkedWord = BitsWord<3>::byOrd(0);
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(updatedWord);
    const auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterEnd) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto updatedWord = BitsWord<3>::byOrd(2);
    const auto checkedWord = BitsWord<3>::byOrd(7);
    [[maybe_unused]] auto _stats0 = dict.getProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 13);
    EXPECT_EQ(high, 14);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherBeginCenter) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    const auto updatedWord = BitsWord<3>::byOrd(0);
    const auto checkedWord = BitsWord<3>::byOrd(5);
    [[maybe_unused]] const auto _stats0 = dict.getProbabilityStats(updatedWord);
    const auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, Example) {
    auto dict = AdaptiveADictionary<BytesWord<1>>();
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
    EXPECT_EQ(high2 - low2, 1);
    EXPECT_EQ(total2, 254 * 3);
    const auto [low3, high3, total3] = getLetterStats('W');
    EXPECT_EQ(high3 - low3, 1);
    EXPECT_EQ(total3, 253 * 4);
    const auto [low4, high4, total4] = getLetterStats('E');
    EXPECT_EQ(high4 - low4, 1);
    EXPECT_EQ(total4, 252 * 5);
    const auto [low5, high5, total5] = getLetterStats('_');
    EXPECT_EQ(high5 - low5, 251);
    EXPECT_EQ(total5, 251 * 6);
    const auto [low6, high6, total6] = getLetterStats('C');
    EXPECT_EQ(high6 - low6, 1);
    EXPECT_EQ(total6, 251 * 7);
    const auto [low7, high7, total7] = getLetterStats('A');
    EXPECT_EQ(high7 - low7, 1);
    EXPECT_EQ(total7, 250 * 8);
}
