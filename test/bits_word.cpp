#include <gtest/gtest.h>
#include <cstdint>

#include "../include/word/bits_word.hpp"
#include "../include/bits_iterator.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsWord, Construct) {
    auto bw = ga::w::BitsWord<3>();
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromInt) {
    auto num = std::uint8_t{42};
    [[maybe_unused]] auto bw = ga::w::BitsWord<8>(ga::impl::bits_begin(num));
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromIntNotFromStart) {
    auto num = std::uint8_t{42};
    [[maybe_unused]] auto bw = ga::w::BitsWord<5>(ga::impl::bits_begin(num) + 4);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromArray) {
    auto arr = std::array<bool, 5>{true, false, true, true, false};
    [[maybe_unused]] auto bw = ga::w::BitsWord<5>(arr.begin());
}

//----------------------------------------------------------------------------//
TEST(BitsWord, OrdWithArray) {
    auto arr = std::array<bool, 5>{true, false, true, true, false};
    auto bw = ga::w::BitsWord<5>(arr.begin());
    EXPECT_EQ(ga::w::BitsWord<5>::ord(bw), 22);
}
