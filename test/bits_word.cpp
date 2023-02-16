#include <gtest/gtest.h>

#include <cstdint>
#include <boost/range/combine.hpp>

#include <ael/word/bits_word.hpp>
#include <ael/bits_iterator.hpp>

using ga::w::BitsWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsWord, Construct) {
    auto bw = BitsWord<3>();
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromInt) {
    auto num = std::uint8_t{42};
    [[maybe_unused]] auto bw = BitsWord<8>(ga::impl::BitsIterator(num, 0));
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromIntNotFromStart) {
    auto num = std::uint8_t{42};
    [[maybe_unused]] auto bw = BitsWord<5>(ga::impl::BitsIterator(num, 4));
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ConstructFromArray) {
    const auto arr = std::array<bool, 5>{1, 0, 1, 1, 0};
    [[maybe_unused]] auto bw = BitsWord<5>(arr.begin());
}

//----------------------------------------------------------------------------//
TEST(BitsWord, OrdWithArray) {
    const auto arr = std::array<bool, 5>{1, 0, 1, 1, 0};
    const auto bw = BitsWord<5>(arr.begin());
    EXPECT_EQ(BitsWord<5>::ord(bw), 22);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ByOrd) {
    const auto arr = std::array<bool, 3>{1, 0, 1};
    const auto bwByOrd = BitsWord<3>::byOrd(5);
    auto bwFromArr = BitsWord<3>(arr.begin());
    EXPECT_EQ(bwByOrd, bwFromArr);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ByOrd2) {
    auto arr = std::array<bool, 3>{1, 0, 0};
    auto bwByOrd = BitsWord<3>::byOrd(5);
    auto bwFromArr = BitsWord<3>(arr.begin());
    EXPECT_NE(bwByOrd, bwFromArr);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, BitsOut) {
    auto arr = std::array<bool, 7>{1, 0, 0, 1, 1, 0, 1};
    auto bw = BitsWord<7>(arr.begin());
    std::vector<bool> testOut;
    auto testBackInserter = std::back_inserter(testOut);
    bw.bitsOut(testBackInserter);
    EXPECT_EQ(testOut.size(), 7);
    for (const auto& [arrI, vecI] : boost::range::combine(arr, testOut)) {
        EXPECT_EQ(arrI, vecI);
    }
}

//----------------------------------------------------------------------------//
TEST(BitsWord, BitsOutLongerWord) {
    auto arr = std::array<bool, 13>{1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0};
    auto bw = BitsWord<13>(arr.begin());
    std::vector<bool> testOut;
    auto testBackInserter = std::back_inserter(testOut);
    bw.bitsOut(testBackInserter);
    EXPECT_EQ(testOut.size(), 13);
    for (const auto& [arrI, vecI] : boost::range::combine(arr, testOut)) {
        EXPECT_EQ(arrI, vecI);
    }
}

//----------------------------------------------------------------------------//
TEST(BitsWord, ByOrdOrd) {
    const auto arr = std::array<bool, 5>{1, 0, 0, 1, 1};
    const auto word = BitsWord<5>(arr.begin());
    const auto ord = BitsWord<5>::ord(word);

    EXPECT_EQ(BitsWord<5>::byOrd(ord), word);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, OrdByOrd) {
    const auto ord = 42;
    const auto word = BitsWord<7>::byOrd(ord);

    EXPECT_EQ(BitsWord<7>::ord(word), ord);
}

//----------------------------------------------------------------------------//
TEST(BitsWord, Output) {
    const auto arr = std::array<bool, 5>{1, 1, 0, 0, 1};
    const auto word = BitsWord<5>(arr.begin());

    auto testOutStream = std::ostringstream();
    testOutStream << word;

    EXPECT_EQ(testOutStream.str(), "11001");
}

//----------------------------------------------------------------------------//
TEST(BitsWord, LongerWord) {
    const auto arr = std::array<bool, 11>{1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0};
    const auto word = BitsWord<11>(arr.begin());

    auto testOutStream = std::ostringstream();
    testOutStream << word;

    EXPECT_EQ(testOutStream.str(), "11001110100");
}


