#include <gtest/gtest.h>

#include "../include/dictionary/static_dictionary.hpp"
#include "../include/word/bytes_word.hpp"
#include "../include/word/bits_word.hpp"
#include "../include/dictionary/traits.hpp"

using ga::dict::StaticDictionary;
using ga::w::BytesWord;
using ga::w::BitsWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
static_assert(!ga::dict::traits::needWordIncrease<StaticDictionary<BytesWord<1>>>);

//----------------------------------------------------------------------------//
static_assert(ga::dict::traits::needSerialize<StaticDictionary<BytesWord<1>>, ga::ByteDataConstructor>);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(StaticDictionary, ConstructFromCumulativeNumFound) {
    using Dict = StaticDictionary<BytesWord<1>>;
    auto cumulFreq = std::array<std::pair<BytesWord<1>, std::uint64_t>, 0>{};
    auto dict = Dict::fromCountMap(cumulFreq);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes1) {
    auto countMap = std::array<std::pair<BytesWord<2>, std::uint64_t>, 2>{
        std::make_pair(BytesWord<2>::byOrd(42), 37),
        std::make_pair(BytesWord<2>::byOrd(112), 5)
    };

    auto dict = StaticDictionary<BytesWord<2>>::fromCountMap(countMap);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = BytesWord<2>(word1Bytes);

    EXPECT_EQ(dict.getWord(37), word1);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2BytesFromFreq) {
    using Dict = StaticDictionary<BitsWord<3>>;

    auto countMap = std::array {
        std::make_pair(BitsWord<3>::byOrd(0), std::uint64_t(2)),  // 2
        std::make_pair(BitsWord<3>::byOrd(1), std::uint64_t(3)),  // 5
        std::make_pair(BitsWord<3>::byOrd(2), std::uint64_t(1)),  // 6
        std::make_pair(BitsWord<3>::byOrd(3), std::uint64_t(2)),  // 8
        std::make_pair(BitsWord<3>::byOrd(4), std::uint64_t(5)),  // 13
        std::make_pair(BitsWord<3>::byOrd(5), std::uint64_t(9)),  // 22
        std::make_pair(BitsWord<3>::byOrd(6), std::uint64_t(3)),  // 25
        std::make_pair(BitsWord<3>::byOrd(7), std::uint64_t(0))   // 25
    };

    auto dict = Dict::fromCountMap(countMap);

    EXPECT_EQ(dict.getWord(5), BitsWord<3>::byOrd(2));
    EXPECT_EQ(dict.getWord(15), BitsWord<3>::byOrd(5));
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes2) {
    auto countMap = std::array {
        std::make_pair(BytesWord<3>::byOrd(42), std::uint64_t(37)),
        std::make_pair(BytesWord<3>::byOrd(112), std::uint64_t(42))
    };

    auto dict = StaticDictionary<BytesWord<3>>::fromCountMap(countMap);

    auto word = BytesWord<3>::byOrd(42);

    EXPECT_EQ(dict.getWord(36), word);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundLow) {
    auto countMap = std::array {
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(37)),
        std::make_pair(BytesWord<2>::byOrd(112), std::uint64_t(5))
    };

    auto dict = StaticDictionary<BytesWord<2>>::fromCountMap(countMap);

    auto word1 = BytesWord<2>::byOrd(112);
    auto word2 = BytesWord<2>::byOrd(113);

    auto [low1, _0, _1] = dict.getProbabilityStats(word1);
    auto [_2, high2, _3] = dict.getProbabilityStats(word2);

    EXPECT_EQ(low1, 37);
    EXPECT_EQ(high2, 42);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundZero) {
    auto countMap = std::array {
        std::make_pair(BytesWord<1>::byOrd(42), std::uint64_t(37)),
        std::make_pair(BytesWord<1>::byOrd(112), std::uint64_t(42))
    };

    auto dict = StaticDictionary<BytesWord<1>>::fromCountMap(countMap);
    auto word = BytesWord<1>::byOrd(1);

    auto [low, _0, _1] = dict.getProbabilityStats(word);

    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundHigh) {
    auto countMap = std::array {
        std::make_pair(BytesWord<2>::byOrd(42), std::uint64_t(37)),
        std::make_pair(BytesWord<2>::byOrd(112), std::uint64_t(5))
    };

    auto dict = StaticDictionary<BytesWord<2>>::fromCountMap(countMap);

    auto word1Bytes = std::array{std::byte{0}, std::byte{111}};
    auto word1 = BytesWord<2>(word1Bytes);

    auto word2Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word2 = BytesWord<2>(word2Bytes);

    auto [_0, high1, _1] = dict.getProbabilityStats(word1);
    auto [_2, high2, _3] = dict.getProbabilityStats(word2);

    EXPECT_EQ(high1, 37);
    EXPECT_EQ(high2, 42);
}
