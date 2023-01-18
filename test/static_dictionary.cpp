#include <gtest/gtest.h>

#include "../include/dictionary/static_dictionary.hpp"
#include "../include/word/bytes_word.hpp"
#include "../include/word/bits_word.hpp"
#include "../include/dictionary/traits.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
static_assert(!ga::dict::traits::needWordIncrease<
                  ga::dict::StaticDictionary<ga::w::BytesWord<1>>
              >);

//----------------------------------------------------------------------------//
static_assert(ga::dict::traits::needSerialize<
                  ga::dict::StaticDictionary<ga::w::BytesWord<1>>,
                  ga::ByteDataConstructor
              >);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(StaticDictionary, ConstructFromCumulativeNumFound) {
    using Dict = ga::dict::StaticDictionary<ga::w::BytesWord<1>>;
    auto cumulFreq = std::array<std::uint64_t, 256>{};
    auto dict = Dict::fromCumulativeCounts(cumulFreq);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, ConstructFromCounts) {
    using Dict = ga::dict::StaticDictionary<ga::w::BytesWord<1>>;
    auto freq = std::array<std::uint64_t, 256>{};
    auto dict = Dict::fromCounts(freq);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes1) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesWord<2>>::fromCumulativeCounts(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = ga::w::BytesWord<2>(word1Bytes);

    EXPECT_EQ(dict.getWord(37), word1);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2BytesFromFreq) {
    using Dict = ga::dict::StaticDictionary<ga::w::BitsWord<3>>;

    auto freq = std::array<std::uint64_t, 8>{ 2, 3, 1, 2, 5,  9,  3 };
                                           // 3, 5, 6, 8, 13, 22, 25
    auto dict = Dict::fromCounts(freq);

    auto word1 = ga::w::BitsWord<3>(ga::impl::bits_begin(std::byte{0b010}) + 5);
    auto word2 = ga::w::BitsWord<3>(ga::impl::bits_begin(std::byte{0b101}) + 5);

    EXPECT_EQ(dict.getWord(5), word1);
    EXPECT_EQ(dict.getWord(15), word2);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes2) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesWord<2>>::fromCumulativeCounts(freq);

    auto wordBytes = std::array{std::byte{0}, std::byte{42}};
    auto word = ga::w::BytesWord<2>(wordBytes);

    EXPECT_EQ(dict.getWord(36), word);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundLow) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesWord<2>>::fromCumulativeCounts(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = ga::w::BytesWord<2>(word1Bytes);

    auto word2Bytes = std::array<std::byte, 2>{std::byte{0}, std::byte{113}};
    auto word2 = ga::w::BytesWord<2>(word2Bytes);

    auto [low1, _0, _1] = dict.getProbabilityStats(word1);
    auto [_2, high2, _3] = dict.getProbabilityStats(word2);

    EXPECT_EQ(low1, 37);
    EXPECT_EQ(high2, 42);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundZero) {
    auto freq = std::vector<std::uint64_t>(256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesWord<1>>::fromCumulativeCounts(freq);

    auto wordBytes = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesWord<1>(wordBytes);

    auto [low, _0, _1] = dict.getProbabilityStats(word);

    EXPECT_EQ(low, 0);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundHigh) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesWord<2>>::fromCumulativeCounts(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{111}};
    auto word1 = ga::w::BytesWord<2>(word1Bytes);

    auto word2Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word2 = ga::w::BytesWord<2>(word2Bytes);

    auto [_0, high1, _1] = dict.getProbabilityStats(word1);
    auto [_2, high2, _3] = dict.getProbabilityStats(word2);

    EXPECT_EQ(high1, 37);
    EXPECT_EQ(high2, 42);
}
