#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <boost/range/combine.hpp>

#include <ael/flow/bytes_word_flow.hpp>

using ga::fl::BytesWordFlow;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Construct) {
    const auto data = std::array<std::byte, 15>{};
    [[maybe_unused]] const auto wf = BytesWordFlow<2>(data);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize0) {
    const auto data = std::array<std::byte, 15>{};
    const auto wf = BytesWordFlow<2>(data);
    EXPECT_EQ(wf.getTail().size(), 8);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize1) {
    const auto data = std::array<std::byte, 15>{};
    const auto wf = BytesWordFlow<3>(data);
    EXPECT_EQ(wf.getTail().size(), 0);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, TailSize2) {
    const auto data = std::array<std::byte, 3>{};
    const auto wf = BytesWordFlow<4>(data);
    EXPECT_EQ(wf.getTail().size(), 3 * 8);
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Tail) {
    const auto data = std::array{ std::byte{0b00011101} };
    const auto wf = BytesWordFlow<4>(data);
    const auto expectedTail = std::array<bool, 8>{0, 0, 0, 1, 1, 1, 0, 1};
    const auto foundTail = wf.getTail();
    for (const auto& [expected, found] : boost::range::combine(expectedTail, foundTail)) {
        EXPECT_EQ(expected, found);
    }
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, Iterate) {
    const auto data = std::array<std::byte, 3>{};
    const auto wf = BytesWordFlow<4>(data);
    for (const auto& w: wf) {}
}

//----------------------------------------------------------------------------//
TEST(BytesWordFlow, IterateCheckNomberOfWords) {
    const auto data = std::array<std::byte, 14>{};
    const auto wf = BytesWordFlow<4>(data);
    std::size_t n = 0;
    for (const auto& w: wf) {
        ++n;
    }
    EXPECT_EQ(n, 3);
}

