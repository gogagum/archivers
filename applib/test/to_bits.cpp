#include <algorithm>
#include <applib/flow/impl/to_bits.hpp>
#include <array>
#include <gtest/gtest.h>
#include <list>
#include <ranges>

TEST(ToBits, OneElementArrayToBitsBytes) {
  const auto data = std::array<std::byte, 1>{std::byte{0}};

  const auto bView = ToBits<std::byte>::perform(data);
  static_assert(std::ranges::random_access_range<decltype(bView)>);
  static_assert(std::ranges::sized_range<decltype(bView)>);

  const auto expected = std::array<std::byte, 8>{};
  EXPECT_TRUE(std::ranges::equal(expected, bView));
}

TEST(ToBits, OneElementArrayToBitsU8) {
  const auto data = std::array<std::byte, 1>{std::byte{0}};

  const auto bView = ToBits<std::uint8_t>::perform(data);
  static_assert(std::ranges::random_access_range<decltype(bView)>);
  static_assert(std::ranges::sized_range<decltype(bView)>);

  const auto expected = std::array<std::uint8_t, 8>{};
  EXPECT_TRUE(std::ranges::equal(expected, bView));
}

TEST(ToBits, ListToBitsBytes) {
  const auto l = std::list<std::byte>{std::byte{0}};

  auto bView = ToBits<std::byte>::perform(l);
  static_assert(std::ranges::input_range<decltype(bView)>);

  bView.begin();

  static_assert(std::ranges::sized_range<decltype(bView)>);

  const auto expected = std::array<std::byte, 8>{};
  EXPECT_TRUE(std::ranges::equal(expected, bView));
}

TEST(ToBits, ListToBitsU8) {
  const auto l = std::list<std::byte>{std::byte{0}};

  auto bView = ToBits<std::uint8_t>::perform(l);
  static_assert(std::ranges::input_range<decltype(bView)>);

  bView.begin();

  static_assert(std::ranges::sized_range<decltype(bView)>);

  const auto expected = std::array<std::uint8_t, 8>{};
  EXPECT_TRUE(std::ranges::equal(expected, bView));
}

TEST(ToBits, BytesArrayToU8Bits) {
  const auto data = std::array<std::byte, 1>{std::byte{1}};

  const auto bView = ToBits<std::uint8_t>::perform(data);
  static_assert(std::ranges::random_access_range<decltype(bView)>);
  static_assert(std::ranges::sized_range<decltype(bView)>);

  const auto expected = std::array<std::uint8_t, 8>{0, 0, 0, 0, 0, 0, 0, 1};
  EXPECT_TRUE(std::ranges::equal(expected, bView));
}

TEST(ToBits, U8Array1ToBits) {
  const auto data = std::array<std::uint8_t, 1>{10};
  const auto bView = ToBits<std::uint8_t>::perform(data);
  const auto expected = std::array<std::uint8_t, 8>{0, 0, 0, 0, 1, 0, 1, 0};
  EXPECT_TRUE(std::ranges::equal(expected, bView));
}
