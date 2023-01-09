#include <gtest/gtest.h>

#include "../include/bits_iterator.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsIterator, Construct) {
    std::uint64_t num;
    auto iter = ga::impl::BitsIterator(num, 3);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref) {
    std::uint64_t num = 0;
    auto iter = ga::impl::BitsIterator(num, 3);
    EXPECT_FALSE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref2) {
    auto num = std::byte{0b11110000};
    auto iter = ga::impl::BitsIterator(num, 2);
    EXPECT_TRUE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref3) {
    auto num = std::byte{0b11110000};
    auto iter = ga::impl::BitsIterator(num, 7);
    EXPECT_FALSE(*iter);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, Construct) {
    auto num = 42;
    ga::impl::make_bits_iterator_range(num);
}

//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, Size) {
    std::uint32_t num = 42;
    auto rng = ga::impl::make_bits_iterator_range(num);
    EXPECT_EQ(std::size(rng), 32);
}

//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, Iterate) {
    std::uint32_t num = 42;
    std::size_t cnt = 0;
    for ([[maybe_unused]] bool bit : ga::impl::make_bits_iterator_range(num)) {
        ++cnt;
    }
    EXPECT_EQ(cnt, 32);
}

//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, BoolVec) {
    std::uint32_t num = 0xFF00A5C3;  // 1111 1111 0000 0000 1100 0101 1010 0011
    auto bitsRng = ga::impl::make_bits_iterator_range(num);
    auto vec = std::vector(bitsRng.begin(), bitsRng.end());
    EXPECT_EQ(vec[4], true);
    EXPECT_EQ(vec[7], true);
    EXPECT_EQ(vec[9], false);
    EXPECT_EQ(vec[12], false);
    EXPECT_EQ(vec[16], true);
}
