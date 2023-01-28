#include <gtest/gtest.h>
#include <array>
#include <cstddef>

#include "flow/bytes_word_flow.hpp"

using ga::fl::BytesWordFlow;
using ga::w::BytesWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Construct) {
    auto data = std::array<std::byte, 15>{};
    [[maybe_unused]] auto wf = BytesWordFlow<2>(data);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize0) {
    auto data = std::array<std::byte, 15>{};
    auto wf = BytesWordFlow<2>(data);
    EXPECT_EQ(wf.getTail().size(), 8);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize1) {
    auto data = std::array<std::byte, 15>{};
    auto wf = BytesWordFlow<3>(data);
    EXPECT_EQ(wf.getTail().size(), 0);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize2) {
    auto data = std::array<std::byte, 3>{};
    auto wf = BytesWordFlow<4>(data);
    EXPECT_EQ(wf.getTail().size(), 3 * 8);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Iterate) {
    auto data = std::array<std::byte, 3>{};
    auto wf = BytesWordFlow<4>(data);
    for (const auto& w: wf) {}
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, IterateCheckNomberOfWords) {
    auto data = std::array<std::byte, 14>{};
    auto wf = BytesWordFlow<4>(data);
    std::size_t n = 0;
    for (const auto& w: wf) {
        ++n;
    }
    EXPECT_EQ(n, 3);
}
