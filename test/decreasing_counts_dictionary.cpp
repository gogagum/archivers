#include <gtest/gtest.h>

#include "dictionary/decreasing_counts_dictionary.hpp"

using ga::dict::DecreasingCountDictionary;
using ga::w::UIntWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, Construct) {
    [[maybe_unused]] auto dict = DecreasingCountDictionary(42);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, TotalCount) {
    auto dict = DecreasingCountDictionary(42);
    EXPECT_EQ(dict.getTotalWordsCount(), 42);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, TotalAfterUpdate) {
    auto dict = DecreasingCountDictionary<std::uint32_t>(42);
    [[maybe_unused]] auto _ = dict.getProbabilityStats(UIntWord<std::uint32_t>(14));
    EXPECT_EQ(dict.getTotalWordsCount(), 14);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, StatsAfterUpdate) {
    auto dict = DecreasingCountDictionary<std::uint32_t>(42);
    auto [low, high, total] = dict.getProbabilityStats(UIntWord<std::uint32_t>(14));
    EXPECT_EQ(low, 13);
    EXPECT_EQ(high, 14);
    EXPECT_EQ(total, 42);
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, NonDecreasingWord0) {
    auto dict = DecreasingCountDictionary<std::uint32_t>(42);
    EXPECT_THROW( \
        [[maybe_unused]] auto stats = \
            dict.getProbabilityStats(UIntWord<std::uint32_t>(56)), \
        DecreasingCountDictionary<std::uint32_t>::NonDecreasingWord \
    );
}

//----------------------------------------------------------------------------//
TEST(DecreasingCountDictionary, NonDecreasingWord1) {
    auto dict = DecreasingCountDictionary<std::uint32_t>(42);
    EXPECT_THROW( \
        [[maybe_unused]] auto word = dict.getWord(56), \
        DecreasingCountDictionary<std::uint32_t>::NonDecreasingWord \
    );
}

