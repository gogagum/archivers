#include <gtest/gtest.h>
#include <array>
#include <cstddef>

#include "flow/bits_word_flow.hpp"

using ga::fl::BitsWordFlow;
using ga::w::BitsWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsWordFlow, Construct) {
    auto data = std::vector<std::byte>(23);
    [[maybe_unused]] auto wf = BitsWordFlow<3>(data);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, Iterate) {
    auto data = std::vector<std::byte>(23);
    auto wf = BitsWordFlow<3>(data);

    for ([[maybe_unused]] auto word: wf) {}
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, IterateConting0) {
    auto data = std::vector<std::byte>(2);
    auto wf = BitsWordFlow<3>(data);
    auto n = std::size_t{0};

    for ([[maybe_unused]] auto word: wf) {
        ++n;
    }

    EXPECT_EQ(n, 5);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, IterateConting1) {
    auto data = std::vector<std::byte>(3);
    auto wf = BitsWordFlow<3>(data);
    auto n = std::size_t{0};

    for ([[maybe_unused]] auto word: wf) {
        ++n;
    }

    EXPECT_EQ(n, 8);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, TailSize0) {
    auto data = std::array<std::byte, 15>{};
    auto wf = BitsWordFlow<2>(data);
    EXPECT_EQ(wf.getTail().size(), 0);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, TailSize1) {
    auto data = std::array<std::byte, 1>{};
    auto wf = BitsWordFlow<3>(data);
    EXPECT_EQ(wf.getTail().size(), 2);
}

