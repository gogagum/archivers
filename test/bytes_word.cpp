#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <vector>
#include <boost/range/combine.hpp>

#include "word/bytes_word.hpp"
#include "word/word_ord_comp.hpp"
#include "byte_data_constructor.hpp"

using ga::w::BytesWord;
using ga::w::WordOrdComp;

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(!ga::w::BytesOutputTransformable<std::vector<bool>::iterator>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(ga::w::BytesOutputTransformable<typename ga::ByteDataConstructor::BitBackInserter>);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static_assert(!ga::w::BytesOutputTransformable<typename ga::ByteDataConstructor::ByteBackInserter>);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesSymbol, Construct) {
    std::array<std::byte, 5> testData;
    [[maybe_unused]] auto sym = BytesWord<5>(testData.data());
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, ConstructFromArray) {
    std::array<std::byte, 5> testData;
    [[maybe_unused]] auto sym = BytesWord<5>(testData);
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BytesSymbolsOrder1) {
    auto testData1 = std::array{ std::byte{0b00000000}, std::byte{0b11111111} };
    auto testData2 = std::array{ std::byte{0b11111111}, std::byte{0b11111111} };

    auto sym1 = BytesWord<2>(testData1.data());
    auto sym2 = BytesWord<2>(testData2.data());

    auto comp = ga::w::WordOrdComp<BytesWord<2>>();

    ASSERT_TRUE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BytesSymbolsOrder2) {
    auto testData1 = std::array{ std::byte{0b00000111} };
    auto testData2 = std::array{ std::byte{0b00001111} };

    auto sym1 = BytesWord<1>(testData1.data());
    auto sym2 = BytesWord<1>(testData2.data());

    auto comp = ga::w::WordOrdComp<BytesWord<1>>();

    ASSERT_TRUE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BytesSymbolsOrder3) {
    auto testData1 = std::array{ std::byte{0b00001111} };
    auto testData2 = std::array{ std::byte{0b00000111} };

    auto sym1 = BytesWord<1>(testData1);
    auto sym2 = BytesWord<1>(testData2);

    auto comp = ga::w::WordOrdComp<BytesWord<1>>();

    ASSERT_FALSE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BytesSymbolsOrder4) {
    auto testData1 = std::array{ std::byte{0b10000000}, std::byte{0b00000000} };
    auto testData2 = std::array{ std::byte{0b00001111}, std::byte{0b11111111} };

    auto sym1 = BytesWord<2>(testData1);
    auto sym2 = BytesWord<2>(testData2);

    auto comp = WordOrdComp<BytesWord<2>>();

    ASSERT_FALSE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BytesSymbolsInMap) {
    using MapToInt64 = std::map<BytesWord<1>, std::int64_t, WordOrdComp<BytesWord<1>>>;

    auto testData1 = std::array{ std::byte{0b00000000} };
    auto testData2 = std::array{ std::byte{0b11111111} };

    auto sym1 = BytesWord<1>(testData1);
    auto sym2 = BytesWord<1>(testData2);

    auto m = MapToInt64();

    m[sym1] = 37;
    m[sym2] = 42;

    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m[sym1], 37);
    EXPECT_EQ(m.lower_bound(sym1)->second, 37);
    EXPECT_EQ(m.upper_bound(sym1)->second, 42);
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BytesSymbolsInMapUpperBoundOfSymNotInMap) {
    using MapToInt64 =
        std::map<BytesWord<1>, std::int64_t, WordOrdComp<BytesWord<1>>>;

    auto testData1 = std::array{ std::byte{0b00000000} };
    auto testData2 = std::array{ std::byte{0b00001111} };
    auto testData3 = std::array{ std::byte{0b11111111} };

    auto sym1 = BytesWord<1>(testData1);
    auto sym2 = BytesWord<1>(testData2);
    auto sym3 = BytesWord<1>(testData3);

    auto m = MapToInt64();

    m[sym1] = 37;
    m[sym3] = 42;

    EXPECT_EQ(m.size(), 2);

    EXPECT_EQ(m.lower_bound(sym2)->second, 42);
    EXPECT_EQ(m.upper_bound(sym2)->second, 42);
}

//----------------------------------------------------------------------------//
TEST(BytesSymbol, BitsOut) {
    auto testData = std::array{ std::byte{0b00001111} };
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
