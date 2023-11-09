#include <applib/flow/impl/to_ints.hpp>
#include <array>
#include <gtest/gtest.h>

TEST(ToInts, EmptyArray) {
  auto data = std::array<int, 0>{};
  auto v = ToInts<int, 3>::perform(data);

  static_assert(std::is_same_v<std::ranges::range_value_t<decltype(v)>, int>);

  EXPECT_EQ(v.size(), 0);
}

TEST(ToInts, NonEmptyArrayEmptyResult) {
  auto data = std::array<int, 3>{0, 1, 0};
  auto v = ToInts<int, 4>::perform(data);

  EXPECT_EQ(v.size(), 0);
}

TEST(ToInts, TwoByTwoInFour) {
  auto data = std::array<int, 4>{0, 1, 1, 0};
  auto v = ToInts<int, 2>::perform(data);

  EXPECT_EQ(v.size(), 2);
  EXPECT_TRUE(std::ranges::equal(v, std::array<int, 2>{1, 2}));
}

TEST(ToInts, TwoByTwoInFive) {
  auto data = std::array<int, 5>{0, 1, 1, 0, 1};
  auto v = ToInts<int, 2>::perform(data);

  EXPECT_EQ(v.size(), 2);
  EXPECT_TRUE(std::ranges::equal(v, std::array<int, 2>{1, 2}));
}
