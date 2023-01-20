#include <gtest/gtest.h>

#include "../include/flow/int_range_word_flow.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(IntRangeFlow, construct) {
    auto vec = std::vector<int>{3, 7, 2};
    auto flow = ga::fl::IntegerWordFlow<int, -5, 8>(std::move(vec));
}

//----------------------------------------------------------------------------//
TEST(IntRangeFlow, numWords) {
    auto vec = std::vector<int>{3, 7, 2};
    auto flow = ga::fl::IntegerWordFlow<int, -5, 8>(std::move(vec));
    EXPECT_EQ(flow.getNumberOfWords(), 3);
}

//----------------------------------------------------------------------------//
TEST(IntRangeFlow, iterate) {
    auto vec = std::vector<int>{3, 7, 2};
    auto flow = ga::fl::IntegerWordFlow<int, -5, 4>(std::move(vec));

    for ([[maybe_unused]] auto word: flow) {

    }
}
