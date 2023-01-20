#include <gtest/gtest.h>
#include <cstdint>
#include <boost/range/combine.hpp>

#include "word/bits_word.hpp"
#include "bits_iterator.hpp"

using ga::w::BitsWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsWord, Construct) {
    auto bw = BitsWord<3>();
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromInt) {
    auto num = std::uint8_t{42};
    [[maybe_unused]] auto bw = BitsWord<8>(ga::impl::bits_begin(num));
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromIntNotFromStart) {
    auto num = std::uint8_t{42};
    [[maybe_unused]] auto bw = BitsWord<5>(ga::impl::bits_begin(num) + 4);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromArray) {
    auto arr = std::array<bool, 5>{true, false, true, true, false};
    [[maybe_unused]] auto bw = BitsWord<5>(arr.begin());
}

//----------------------------------------------------------------------------//
TEST(BitsWord, OrdWithArray) {
    auto arr = std::array<bool, 5>{true, false, true, true, false};
    auto bw = BitsWord<5>(arr.begin());
    EXPECT_EQ(BitsWord<5>::ord(bw), 22);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ByOrd) {
    auto arr = std::array<bool, 3>{true, false, true};
    auto bwByOrd = BitsWord<3>::byOrd(5);
    auto bwFromArr = BitsWord<3>(arr.begin());
    EXPECT_EQ(bwByOrd, bwFromArr);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ByOrd2) {
    auto arr = std::array<bool, 3>{true, false, false};
    auto bwByOrd = BitsWord<3>::byOrd(5);
    auto bwFromArr = BitsWord<3>(arr.begin());
    EXPECT_NE(bwByOrd, bwFromArr);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, BitsOut) {
    auto arr = std::array<bool, 7>{true, false, false, true, true, false, true};
    auto bw = BitsWord<7>(arr.begin());
    std::vector<bool> testOut;
    auto testBackInserter = std::back_inserter(testOut);
    bw.bitsOut(testBackInserter);
    EXPECT_EQ(testOut.size(), 7);
    for (const auto& [arrI, vecI] : boost::range::combine(arr, testOut)) {
        EXPECT_EQ(arrI, vecI);
    }
}

