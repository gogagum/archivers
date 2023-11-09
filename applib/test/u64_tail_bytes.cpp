#include <gtest/gtest.h>

#include <applib/u64_tail_bytes.hpp>

namespace {

struct U64TailBytesTestCase {
  std::uint64_t i;
  std::size_t bytesCnt;
  std::vector<std::byte> expected;
  std::string description;
};

class U64TailBytesParametrizedTest
    : public testing::TestWithParam<U64TailBytesTestCase> {};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,
// cert-err58-cpp, bugprone-branch-clone)

TEST_P(U64TailBytesParametrizedTest, UnitTest) {
  const auto& params = U64TailBytesParametrizedTest::GetParam();
  EXPECT_TRUE(std::ranges::equal(u64_tail_bytes(params.i, params.bytesCnt),
                                 params.expected));
}

const auto testCases = std::vector<U64TailBytesTestCase>{
    {0, 0, {}, "Empty"},
    {1, 0, {}, "EmptyFromOne"},
    {42, 0, {}, "EmptyFrom42"},
    {0, 1, {std::byte{0x00}}, "OneFalseFromZero"},
    {1, 1, {std::byte{0x01}}, "OneTrueFromOne"},
    {1, 2, {std::byte{0x00}, std::byte{0x01}}, "0x0001"},
    {2, 2, {std::byte{0x00}, std::byte{0x02}}, "0x0002"},
    {0b1001, 4, {std::byte{0b0}, std::byte{0b0}, std::byte{0b0}, std::byte{0x09}}, "0b1001"},
    {0b1011, 4, {std::byte{0b0}, std::byte{0b0}, std::byte{0b0}, std::byte{0x0B}}, "0b1011"},
    {0xAA0101, 3, {std::byte{0xAA}, std::byte{0x01}, std::byte{0x01}}, "0xAA0101"}};

INSTANTIATE_TEST_SUITE_P(U64TailBitsUnitTests, U64TailBytesParametrizedTest,
                         testing::ValuesIn(testCases), [](const auto& param) {
                           return param.param.description;
                         });

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, cert-err58-cpp,
// bugprone-branch-clone)

}  // namespace
