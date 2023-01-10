#include <gtest/gtest.h>

#include "../include/flow/bytes_word_flow.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Construct) {
    auto data = std::array<std::byte, 15>{};
    auto wf = ga::fl::BytesWordFlow<ga::w::BytesSymbol<2>>(data.data(), 15);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize0) {
    auto data = std::array<std::byte, 15>{};
    auto wf = ga::fl::BytesWordFlow<ga::w::BytesSymbol<2>>(data.data(), 15);
    EXPECT_EQ(wf.getTail().size(), 8);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize1) {
    auto data = std::array<std::byte, 15>{};
    auto wf = ga::fl::BytesWordFlow<ga::w::BytesSymbol<3>>(data.data(), 15);
    EXPECT_EQ(wf.getTail().size(), 0);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize2) {
    auto data = std::array<std::byte, 3>{};
    auto wf = ga::fl::BytesWordFlow<ga::w::BytesSymbol<4>>(data.data(), 3);
    EXPECT_EQ(wf.getTail().size(), 3 * 8);
}
