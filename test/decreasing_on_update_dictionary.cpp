#include <gtest/gtest.h>
#include <array>


#include "dictionary/decreasing_on_update_dictionary.hpp"
#include "word/bytes_word.hpp"

using ga::dict::DecreasingOnUpdateDictionary;
using ga::w::BytesWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, Construct) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, TotalCount) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
    EXPECT_EQ(dict.getTotalWordsCnt(), 32);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, TotalCountEmpty) {
    auto freqMapping = std::array<std::pair<BytesWord<2>, std::uint64_t>, 0>{};
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
    EXPECT_EQ(dict.getTotalWordsCnt(), 0);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, TotalCountAfterDecrease) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
    dict.getProbabilityStats(BytesWord<2>::byOrd(42));

    EXPECT_EQ(dict.getTotalWordsCnt(), 31);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, CountAfterDecrease) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
    dict.getProbabilityStats(BytesWord<2>::byOrd(42));

    auto [low, high, _0] = dict.getProbabilityStats(BytesWord<2>::byOrd(42));

    EXPECT_EQ(low, 0);
    EXPECT_EQ(high, 14);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, QueryForAnEmptyWord) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
    EXPECT_THROW(dict.getProbabilityStats(BytesWord<2>::byOrd(13)),
                 DecreasingOnUpdateDictionary<BytesWord<2>>::NoSuchWord);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, CountAfterDecreaseTwice) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);
    dict.getProbabilityStats(BytesWord<2>::byOrd(42));

    auto [low0, high0, _0] = dict.getProbabilityStats(BytesWord<2>::byOrd(42));

    EXPECT_EQ(low0, 0);
    EXPECT_EQ(high0, 14);

    auto [low1, high1, _1] = dict.getProbabilityStats(BytesWord<2>::byOrd(105));

    EXPECT_EQ(low1, 13);
    EXPECT_EQ(high1, 30);
}

//----------------------------------------------------------------------------//
TEST(DecreasingOnUpdateDictionary, GetWord) {
    auto freqMapping = std::array{
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(15)),
        std::make_pair(BytesWord<2>::byOrd(105), std::uint64_t(17))
    };
    auto dict = DecreasingOnUpdateDictionary<BytesWord<2>>(freqMapping);

    auto word = dict.getWord(12);
    EXPECT_EQ(word, BytesWord<2>::byOrd(42));
}
