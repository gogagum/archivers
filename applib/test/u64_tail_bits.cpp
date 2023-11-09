#include <gtest/gtest.h>

#include <applib/u64_tail_bits.hpp>

namespace {

struct U64TailBitsTestCase {
  std::uint64_t i;
  std::size_t bitsCnt;
  std::vector<bool> expected;
  std::string description;
};

class U64TailBitsParametrizedTest
    : public testing::TestWithParam<U64TailBitsTestCase> {};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,
// cert-err58-cpp, bugprone-branch-clone)

TEST_P(U64TailBitsParametrizedTest, UnitTest) {
  const auto& params = U64TailBitsParametrizedTest::GetParam();
  EXPECT_TRUE(std::ranges::equal(u64_tail_bits(params.i, params.bitsCnt),
                                 params.expected));
}

const auto testCases = std::vector<U64TailBitsTestCase>{
    {0, 0, {}, "Empty"},
    {1, 0, {}, "EmptyFromOne"},
    {42, 0, {}, "EmptyFrom42"},
    {0, 1, {false}, "OneFalseFromZero"},
    {1, 1, {true}, "OneTrueFromOne"},
    {1, 2, {false, true}, "01"},
    {2, 2, {true, false}, "10"},
    {0b1001, 4, {true, false, false, true}, "1001"},
    {0b1011, 4, {true, false, true, true}, "1011"}};

INSTANTIATE_TEST_SUITE_P(U64TailBitsUnitTests, U64TailBitsParametrizedTest,
                         testing::ValuesIn(testCases), [](const auto& param) {
                           return param.param.description;
                         });

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, cert-err58-cpp,
// bugprone-branch-clone)

}  // namespace
