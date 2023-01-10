#include <gtest/gtest.h>
#include <array>
#include <cstddef>

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

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Iterate) {
    auto data = std::array<std::byte, 3>{};
    auto wf = ga::fl::BytesWordFlow<ga::w::BytesSymbol<4>>(data.data(), 3);
    for (const auto& w: wf) {}
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, IterateCheckNomberOfWords) {
    auto data = std::array<std::byte, 14>{};
    auto wf = ga::fl::BytesWordFlow<ga::w::BytesSymbol<4>>(data.data(), 14);
    std::size_t n = 0;
    for (const auto& w: wf) {
        ++n;
    }
    EXPECT_EQ(n, 3);
}
