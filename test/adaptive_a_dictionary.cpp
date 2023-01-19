#include <gtest/gtest.h>

#include "../include/dictionary/adaptive_a_dictionary.hpp"
#include "../include/word/bytes_word.hpp"
#include "../include/word/bits_word.hpp"

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
            dict.getWordProbabilityStats(BytesWord<2>(wordData));
}

//----------------------------------------------------------------------------//
TEST(AdapriveADictionary, GetStatsOnStartCenter) {
    auto dict = AdaptiveADictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b110 };  // 6
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getWordProbabilityStats(word);
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
    auto [low, high, total] = dict.getWordProbabilityStats(word);
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
    auto [low, high, total] = dict.getWordProbabilityStats(word);
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
    [[maybe_unused]] auto stats0 = dict.getWordProbabilityStats(word);
    auto [low, high, total] = dict.getWordProbabilityStats(word);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
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
    [[maybe_unused]] auto stats0 = dict.getWordProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getWordProbabilityStats(checkedWord);
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
    [[maybe_unused]] auto stats0 = dict.getWordProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getWordProbabilityStats(checkedWord);
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
    [[maybe_unused]] auto stats0 = dict.getWordProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getWordProbabilityStats(checkedWord);
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
    [[maybe_unused]] auto stats0 = dict.getWordProbabilityStats(updatedWord);
    auto [low, high, total] = dict.getWordProbabilityStats(checkedWord);
    EXPECT_EQ(low, 11);
    EXPECT_EQ(high, 12);
    EXPECT_EQ(total, 14);
}
