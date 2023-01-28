#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <vector>
#include <boost/range/combine.hpp>

#include "word/bytes_word.hpp"
#include "byte_data_constructor.hpp"

using ga::w::BytesWord;

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(!ga::w::BytesOutputTransformable<std::vector<bool>::iterator>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(ga::w::BytesOutputTransformable<typename ga::ByteDataConstructor::BitBackInserter>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(!ga::w::BytesOutputTransformable<typename ga::ByteDataConstructor::ByteBackInserter>);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesWord, Construct) {
    std::array<std::byte, 5> testData;
    [[maybe_unused]] auto sym = BytesWord<5>(testData.data());
}

//----------------------------------------------------------------------------//
TEST(BytesWord, ConstructFromArray) {
    std::array<std::byte, 5> testData;
    [[maybe_unused]] auto sym = BytesWord<5>(testData);
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder1) {
    auto testData1 = std::array{ std::byte{0b00000000}, std::byte{0b11111111} };
    auto testData2 = std::array{ std::byte{0b11111111}, std::byte{0b11111111} };

    auto sym1 = BytesWord<2>(testData1.data());
    auto sym2 = BytesWord<2>(testData2.data());

    ASSERT_TRUE(BytesWord<2>::ord(sym1) < BytesWord<2>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder2) {
    const auto sym1 = BytesWord<1>(std::array{ std::byte{0b00000111} });
    const auto sym2 = BytesWord<1>(std::array{ std::byte{0b00001111} });

    ASSERT_TRUE(BytesWord<1>::ord(sym1) < BytesWord<1>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder3) {
    const auto sym1 = BytesWord<1>(std::array{ std::byte{0b00001111} });
    const auto sym2 = BytesWord<1>(std::array{ std::byte{0b00000111} });

    ASSERT_FALSE(BytesWord<1>::ord(sym1) < BytesWord<1>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder4) {
    const auto testData1 = std::array{ std::byte{0b10000000}, std::byte{0b00000000} };
    const auto testData2 = std::array{ std::byte{0b00001111}, std::byte{0b11111111} };

    const auto sym1 = BytesWord<2>(testData1);
    const auto sym2 = BytesWord<2>(testData2);

    ASSERT_FALSE(BytesWord<2>::ord(sym1) < BytesWord<2>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BitsOut) {
    const auto testData = std::array{ std::byte{0b00001111} };
    auto word = BytesWord<1>(testData);

    std::vector<bool> bits;
    auto bitsInserter = std::back_inserter(bits);

    word.bitsOut(bitsInserter);
    EXPECT_EQ(bits.size(), 8);

    auto combinedTestAndTested =
            boost::range::combine(bits,
                                  ga::impl::make_bits_iterator_range(testData));

    for (const auto& [dataI, bitI] : combinedTestAndTested) {
        EXPECT_EQ(dataI, bitI);
    }
}

//----------------------------------------------------------------------------//
TEST(BytesWord, ByOrdOrd) {
    const auto word = BytesWord<2>(
                std::array{std::byte{0b10001110}, std::byte{0b10101010}});

    const auto ord = BytesWord<2>::ord(word);

    EXPECT_EQ(BytesWord<2>::byOrd(ord), word);
}

//----------------------------------------------------------------------------//
TEST(BytesWord, OrdByOrd) {
    const auto ord = 42;
    const auto word = BytesWord<2>::byOrd(ord);

    EXPECT_EQ(BytesWord<2>::ord(word), ord);
}
