#include <gtest/gtest.h>

#include "../include/dictionary/adaptive_a_dictionary.hpp"
#include "../include/word/bytes_word.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, Construct) {
    auto dict = ga::dict::AdaptiveADictionary<ga::w::BytesWord<2>>();
}

//----------------------------------------------------------------------------//
TEST(AdaptiveADictionary, GetStats) {
    auto dict = ga::dict::AdaptiveADictionary<ga::w::BytesWord<2>>();
    auto wordData = std::array{ std::byte{0b00000000}, std::byte{0b00000001} };
    auto [low, high, total] = dict.getWordProbabilityStats(ga::w::BytesWord<2>(wordData));
}
