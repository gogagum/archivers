#include <gtest/gtest.h>
#include <array>
#include <cstddef>

#include "../include/flow/bits_word_flow.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsWordFlow, Construct) {
    auto data = std::vector<std::byte>(23);
    auto wf = ga::fl::BitsWordFlow<ga::w::BitsWord<3>>(std::move(data));
}
