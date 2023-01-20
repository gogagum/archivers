#include <gtest/gtest.h>

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
    auto wordData = std::array{ std::byte{0b00000000}, std::byte{0b00000001} };
    [[maybe_unused]] auto [low, high, total] =
            dict.getProbabilityStats(BytesWord<2>(wordData));
}

//----------------------------------------------------------------------------//
TEST(AdapriveADictionary, GetStatsOnStartCenter) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b110 };  // 6
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 6);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsOnStartEnd) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b111 };  // 7
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 7);
    EXPECT_EQ(high, 8);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsOnStartBegin) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b000 };  // 0
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterUpdate) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b000 };  // 0
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
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
    auto updatedWordData = std::byte{ 0b01000000 };  // 2
    auto checkedWordData = std::byte{ 0b10100000 };  // 5
    auto updatedWord = BitsWord<3>(ga::impl::bits_begin(updatedWordData));
    auto checkedWord = BitsWord<3>(ga::impl::bits_begin(checkedWordData));
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}


//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterBegin) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto updatedWordData = std::byte{ 0b01000000 };  // 2
    auto checkedWordData = std::byte{ 0b00000000 };  // 0
    auto updatedWord = BitsWord<3>(ga::impl::bits_begin(updatedWordData));
    auto checkedWord = BitsWord<3>(ga::impl::bits_begin(checkedWordData));
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherCenterEnd) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto updatedWordData = std::byte{ 0b01000000 };  // 2
    auto checkedWordData = std::byte{ 0b11100000 };  // 7
    auto updatedWord = BitsWord<3>(ga::impl::bits_begin(updatedWordData));
    auto checkedWord = BitsWord<3>(ga::impl::bits_begin(checkedWordData));
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 13);
    EXPECT_EQ(high, 14);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStatsAfterIncreaseOneUpdateOtherBeginCenter) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto updatedWordData = std::byte{ 0b00000000 };  // 0
    auto checkedWordData = std::byte{ 0b10100000 };  // 5
    auto updatedWord = BitsWord<3>(ga::impl::bits_begin(updatedWordData));
    auto checkedWord = BitsWord<3>(ga::impl::bits_begin(checkedWordData));
    [[maybe_unused]] auto stats0 = dict.getProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getProbabilityStats(checkedWord);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, Example) {
    auto dict = AdaptiveADictionary<BytesWord<1>>();
    auto [low0, high0, total0] = dict.getProbabilityStats(BytesWord<1>::byOrd(8));
    EXPECT_EQ(high0 - low0, 1);
    EXPECT_EQ(total0, 256);
    auto [low1, high1, total1] = dict.getProbabilityStats(BytesWord<1>::byOrd(12));
    EXPECT_EQ(high1 - low1, 1);
    EXPECT_EQ(total1, 255 * 2);
    auto [low2, high2, total2] = dict.getProbabilityStats(BytesWord<1>::byOrd(45));
    EXPECT_EQ(high2 - low2, 1);
    EXPECT_EQ(total2, 254 * 3);
    auto [low3, high3, total3] = dict.getProbabilityStats(BytesWord<1>::byOrd(23));
    EXPECT_EQ(high3 - low3, 1);
    EXPECT_EQ(total3, 253 * 4);
    auto [low4, high4, total4] = dict.getProbabilityStats(BytesWord<1>::byOrd(46));
    EXPECT_EQ(high4 - low4, 1);
    EXPECT_EQ(total4, 252 * 5);
    auto [low5, high5, total5] = dict.getProbabilityStats(BytesWord<1>::byOrd(45));
    EXPECT_EQ(high5 - low5, 251);
    EXPECT_EQ(total5, 251 * 6);
}
