#include <gtest/gtest.h>

#include <dictionary/ppmd_dictionary.hpp>
#include <word/bytes_word.hpp>

using ga::dict::PPMDDictionary;
using ga::w::BytesWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(PPMDDictionary, Construct) {
    const auto dict = PPMDDictionary<BytesWord<1>, 3>();
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, WordsCountAfterInit) {
    const auto dict = PPMDDictionary<BytesWord<1>, 3>();
    EXPECT_EQ(dict.getTotalWordsCnt(), 256);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, GetInitProbabilityStatsOrd0) {
    auto dict = PPMDDictionary<BytesWord<1>, 3>();
    const auto [low, high, totalWordsCount] =
        dict.getProbabilityStats(BytesWord<1>::byOrd(0));
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(totalWordsCount, 256);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, GetInitProbabilityStatsOrdEnd) {
    auto dict = PPMDDictionary<BytesWord<1>, 3>();
    const auto [low, high, totalWordsCount] =
        dict.getProbabilityStats(BytesWord<1>::byOrd(255));
    EXPECT_EQ(low, 255);
    EXPECT_EQ(high, 256);
    EXPECT_EQ(totalWordsCount, 256);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, GetInitProbabilityStatsOrdCenter) {
    auto dict = PPMDDictionary<BytesWord<1>, 3>();
    const auto [low, high, totalWordsCount] = 
        dict.getProbabilityStats(BytesWord<1>::byOrd(42));
    EXPECT_EQ(low, 42);
    EXPECT_EQ(high, 43);
    EXPECT_EQ(totalWordsCount, 256);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, DoubleGetStatsSame) {
    auto dict = PPMDDictionary<BytesWord<1>, 3>();
    [[maybe_unused]] const auto _stats =
        dict.getProbabilityStats(BytesWord<1>::byOrd(42));
    const auto [low1, high1, totalWordsCount] =
        dict.getProbabilityStats(BytesWord<1>::byOrd(42));
    EXPECT_EQ(low1, 42);
    EXPECT_EQ(high1, 42 + 255);
    EXPECT_EQ(totalWordsCount, 255 * 2);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, DoubleGetStatsDifferent) {
    auto dict = PPMDDictionary<BytesWord<1>, 3>();
    [[maybe_unused]] const auto _stats =
        dict.getProbabilityStats(BytesWord<1>::byOrd(42));
    const auto [low1, high1, totalWordsCount] =
        dict.getProbabilityStats(BytesWord<1>::byOrd(45));
    EXPECT_EQ(low1, 255 + 45 - 1);
    EXPECT_EQ(high1, 255 + 46 - 1);
    EXPECT_EQ(totalWordsCount, 255 * 2);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, DoubleGetStatsDifferent2) {
    auto dict = PPMDDictionary<BytesWord<1>, 3>();
    [[maybe_unused]] const auto _stats =
        dict.getProbabilityStats(BytesWord<1>::byOrd(42));
    const auto [low1, high1, totalWordsCount] =
        dict.getProbabilityStats(BytesWord<1>::byOrd(43));
    EXPECT_EQ(low1, 255 + 43 - 1);
    EXPECT_EQ(high1, 255 + 44 - 1);
    EXPECT_EQ(totalWordsCount, 255 * 2);
}

//----------------------------------------------------------------------------//
TEST(PPMDDictionary, Example) {
    auto dict = PPMDDictionary<BytesWord<1>, 5>();
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
    EXPECT_EQ(high6 - low6, 1);
    EXPECT_EQ(total6, 255 * 2);
    const auto [low7, high7, total7] = getLetterStats('A');
    EXPECT_EQ(high7 - low7, 6);
    EXPECT_EQ(total7, 250 * 14);
}

