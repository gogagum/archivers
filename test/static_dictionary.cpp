#include <gtest/gtest.h>

#include "../include/dictionary/static_dictionary.hpp"
#include "../include/word/bytes_symbol.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(StaticDictionary, Construct) {
    auto freq = std::array<std::uint64_t, 256>{};
    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<1>>(freq);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes1) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = ga::w::BytesSymbol<2>(word1Bytes);

    EXPECT_EQ(dict.getWord(37), word1);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes2) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto wordBytes = std::array{std::byte{0}, std::byte{42}};
    auto word = ga::w::BytesSymbol<2>(wordBytes);

    EXPECT_EQ(dict.getWord(36), word);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundLow) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = ga::w::BytesSymbol<2>(word1Bytes);

    auto word2Bytes = std::array<std::byte, 2>{std::byte{0}, std::byte{113}};
    auto word2 = ga::w::BytesSymbol<2>(word2Bytes);

    EXPECT_EQ(dict.getLowerCumulativeNumFound(word1), 37);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word2), 42);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundZero) {
    auto freq = std::vector<std::uint64_t>(256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<1>>(freq);

    auto wordBytes = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesSymbol<1>(wordBytes);

    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundHigh) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{111}};
    auto word1 = ga::w::BytesSymbol<2>(word1Bytes);

    auto word2Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word2 = ga::w::BytesSymbol<2>(word2Bytes);

    EXPECT_EQ(dict.getHigherCumulativeNumFound(word1), 37);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word2), 42);
}
