#include <gtest/gtest.h>

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
    auto wordData = std::array{ std::byte{0b00000000}, std::byte{0b00000001} };
    [[maybe_unused]] auto [low, high, total] =
            dict.getProbabilityStats(BytesWord<2>(wordData));
}

//----------------------------------------------------------------------------//
TEST(AdapriveDDictionary, GetStatsOnStartCenter) {
    auto dict = AdaptiveDDictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b110 };  // 6
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 6);
    EXPECT_EQ(high, 7);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, GetStatsOnStartEnd) {
    auto dict = AdaptiveDDictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b111 };  // 7
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 7);
    EXPECT_EQ(high, 8);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}

//----------------------------------------------------------------------------//
TEST(AdaptiveDDictionary, GetStatsOnStartBegin) {
    auto dict = AdaptiveDDictionary<BitsWord<3>>();
    auto testWordData = std::byte{ 0b000 };  // 0
    auto word = BitsWord<3>(ga::impl::bits_end(testWordData) - 3);
    auto [low, high, total] = dict.getProbabilityStats(word);
    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 1);
    EXPECT_EQ(total, 8);
    // Unuform probability. Each letter 1/8.
}
