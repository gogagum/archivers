#include <gtest/gtest.h>
#include <array>
#include <cstddef>

#include <ael/flow/bits_word_flow.hpp>

using ael::fl::BitsWordFlow;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsWordFlow, Construct) {
    const auto data = std::array{std::byte{23}};
    [[maybe_unused]] auto wf = BitsWordFlow<3>(data);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, Iterate) {
    const auto data = std::array{std::byte{23}};
    auto wf = BitsWordFlow<3>(data);

    for ([[maybe_unused]] auto word: wf) {}
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, IterateConting0) {
    const auto data = std::array<std::byte, 2>{};
    auto wf = BitsWordFlow<3>(data);
    auto n = std::size_t{0};

    for ([[maybe_unused]] auto word: wf) {
        ++n;
    }

    EXPECT_EQ(n, 5);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, IterateConting1) {
    const auto data = std::array<std::byte, 3>{};
    auto wf = BitsWordFlow<3>(data);
    auto n = std::size_t{0};

    for ([[maybe_unused]] auto word: wf) {
        ++n;
    }

    EXPECT_EQ(n, 8);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, TailSize0) {
    const auto data = std::array<std::byte, 15>{};
    auto wf = BitsWordFlow<2>(data);
    EXPECT_EQ(wf.getTail().size(), 0);
}

//----------------------------------------------------------------------------//
TEST(BitsWordFlow, TailSize1) {
    const auto data = std::array<std::byte, 1>{};
    auto wf = BitsWordFlow<3>(data);
    EXPECT_EQ(wf.getTail().size(), 2);
}

