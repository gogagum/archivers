#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "bytes_iterator.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesIterator, Construct) {
    const auto num = std::uint32_t{};
    [[maybe_unused]] const auto iter = ga::impl::BytesIterator(num, 1);
}

//----------------------------------------------------------------------------//
TEST(BytesIterator, ArrayBytesIter) {
    const auto arr = std::array{std::byte{0b10101010}, std::byte{0b00001111}};
    const auto iter = ga::impl::BytesIterator(arr, 0);

    EXPECT_EQ(*iter, std::byte{0b00001111});
}

//----------------------------------------------------------------------------//
TEST(BytesIterator, UInt16T) {
    const auto num = std::uint16_t{ 0b1010101000001111 };
    const auto iter0 = ga::impl::BytesIterator(num, 0);
    const auto iter1 = ga::impl::BytesIterator(num, 1);

    EXPECT_EQ(*iter0, std::byte{0b10101010});
    EXPECT_EQ(*iter1, std::byte{0b00001111});
}

//----------------------------------------------------------------------------//
TEST(BytesIterator, UInt32T) {
    const auto num = std::uint32_t{ 0b00000001000000110000011100001111 };
    const auto iter0 = ga::impl::BytesIterator(num, 0);
    const auto iter1 = ga::impl::BytesIterator(num, 1);
    const auto iter2 = ga::impl::BytesIterator(num, 2);
    const auto iter3 = ga::impl::BytesIterator(num, 3);

    EXPECT_EQ(*iter0, std::byte{0b00000001});
    EXPECT_EQ(*iter1, std::byte{0b00000011});
    EXPECT_EQ(*iter2, std::byte{0b00000111});
    EXPECT_EQ(*iter3, std::byte{0b00001111});
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ReverseBytesIterator, Construct) {
    const auto num = std::uint32_t{};
    [[maybe_unused]] const auto iter = ga::impl::ReverseBytesIterator(num, 1);
}

//----------------------------------------------------------------------------//
TEST(ReverseBytesIterator, ArrayBytesIter) {
    const auto arr = std::array{std::byte{0b10101010}, std::byte{0b00001111}};
    const auto iter = ga::impl::ReverseBytesIterator(arr, 0);

    EXPECT_EQ(*iter, std::byte{0b10101010});
}

//----------------------------------------------------------------------------//
TEST(ReverseBytesIterator, UInt16T) {
    const auto num = std::uint16_t{ 0b1010101000001111 };
    const auto iter0 = ga::impl::ReverseBytesIterator(num, 0);
    const auto iter1 = ga::impl::ReverseBytesIterator(num, 1);

    EXPECT_EQ(*iter0, std::byte{0b00001111});
    EXPECT_EQ(*iter1, std::byte{0b10101010});
}

//----------------------------------------------------------------------------//
TEST(ReverseBytesIterator, UInt32T) {
    const auto num = std::uint32_t{ 0b00000001000000110000011100001111 };
    const auto iter0 = ga::impl::ReverseBytesIterator(num, 0);
    const auto iter1 = ga::impl::ReverseBytesIterator(num, 1);
    const auto iter2 = ga::impl::ReverseBytesIterator(num, 2);
    const auto iter3 = ga::impl::ReverseBytesIterator(num, 3);

    EXPECT_EQ(*iter0, std::byte{0b00001111});
    EXPECT_EQ(*iter1, std::byte{0b00000111});
    EXPECT_EQ(*iter2, std::byte{0b00000011});
    EXPECT_EQ(*iter3, std::byte{0b00000001});
}
