#include <gtest/gtest.h>

#include "include/arithmetic_coder_encoded.hpp"
#include "include/bytes_symbol.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, Consruct) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitFalse) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putBit(false);
    EXPECT_EQ(encoded.data()[0], std::byte{0b00000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitTrue) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBits) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(false);
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10010000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBits2) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(false);
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(true);
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10010101});
    EXPECT_EQ(encoded.data()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutByte) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putByte(std::byte{42});
    EXPECT_EQ(encoded.data()[0], std::byte{42});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitAfterByte) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putByte(std::byte{42});
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{42});
    EXPECT_EQ(encoded.data()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutByteAfterBit) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putBit(false);
    encoded.putByte(std::byte{0b10011101});
    EXPECT_EQ(encoded.data()[0], std::byte{0b01001110});
    EXPECT_EQ(encoded.data()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutByte2) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.putByte(std::byte{42});
    encoded.putByte(std::byte{37});
    encoded.putByte(std::byte{73});
    EXPECT_EQ(encoded.data()[0], std::byte{42});
    EXPECT_EQ(encoded.data()[1], std::byte{37});
    EXPECT_EQ(encoded.data()[2], std::byte{73});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutInt64) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    std::int64_t tested =
            0b0000000000100000100000000000100000000000000000100000000000100000;
            //0000000011111111222222223333333344444444555555556666666677777777

    encoded.putT(tested);
    EXPECT_EQ(encoded.bytesSize(), 8);
    EXPECT_EQ(encoded.data()[0], std::byte(0b00000000));
    EXPECT_EQ(encoded.data()[1], std::byte{0b00100000});
    EXPECT_EQ(encoded.data()[2], std::byte{0b10000000});
    EXPECT_EQ(encoded.data()[3], std::byte{0b00001000});
    EXPECT_EQ(encoded.data()[4], std::byte{0b00000000});
    EXPECT_EQ(encoded.data()[5], std::byte{0b00000010});
    EXPECT_EQ(encoded.data()[6], std::byte{0b00000000});
    EXPECT_EQ(encoded.data()[7], std::byte{0b00100000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutUInt32) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    std::uint32_t tested = 0b00000000001000001000000000001000;
                           //00000000111111112222222233333333

    encoded.putT(tested);
    EXPECT_EQ(encoded.bytesSize(), 4);
    EXPECT_EQ(encoded.data()[0], std::byte(0b00000000));
    EXPECT_EQ(encoded.data()[1], std::byte{0b00100000});
    EXPECT_EQ(encoded.data()[2], std::byte{0b10000000});
    EXPECT_EQ(encoded.data()[3], std::byte{0b00001000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutUInt32AfterBit) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    std::uint32_t tested = 0b00000000001000001000000000001000;
                           //00000000111111112222222233333333

    encoded.putBit(false);
    encoded.putT(tested);

    EXPECT_EQ(encoded.bytesSize(), 5);
    EXPECT_EQ(encoded.data()[0], std::byte(0b00000000));
    EXPECT_EQ(encoded.data()[1], std::byte{0b00010000});
    EXPECT_EQ(encoded.data()[2], std::byte{0b01000000});
    EXPECT_EQ(encoded.data()[3], std::byte{0b00000100});
    EXPECT_EQ(encoded.data()[4], std::byte{0b00000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, FillEndFalseEmpty) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.fillEnd(false);

    EXPECT_EQ(encoded.bytesSize(), 0);
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, FillEndTrueEmpty) {
    auto encoded = garchiever::ArithmeticCoderEncoded();
    encoded.fillEnd(true);

    EXPECT_EQ(encoded.bytesSize(), 0);
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, FillEndAfterOneBitTrueEmpty) {
    auto encoded = garchiever::ArithmeticCoderEncoded();

    encoded.putBit(false);
    encoded.fillEnd(true);

    EXPECT_EQ(encoded.bytesSize(), 1);
    EXPECT_EQ(encoded.data()[0], std::byte{0b01111111});
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, Construct) {
    std::array<std::byte, 5> testData;
    auto sym = garchiever::BytesSymbol<5>(testData.data());
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder1) {
    using SymDataT = std::array<std::byte, 2>;
    using SymT = garchiever::BytesSymbol<2>;

    SymDataT testData1 = { std::byte{0b00000000}, std::byte{0b11111111} };
    SymDataT testData2 = { std::byte{0b11111111}, std::byte{0b11111111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto comp = SymT::Order();

    ASSERT_TRUE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder2) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = garchiever::BytesSymbol<1>;

    SymDataT testData1 = { std::byte{0b00000111} };
    SymDataT testData2 = { std::byte{0b00001111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto comp = SymT::Order();

    ASSERT_TRUE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder3) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = garchiever::BytesSymbol<1>;

    SymDataT testData1 = { std::byte{0b00001111} };
    SymDataT testData2 = { std::byte{0b00000111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto comp = SymT::Order();

    ASSERT_FALSE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder4) {
    using SymDataT = std::array<std::byte, 2>;
    using SymT = garchiever::BytesSymbol<2>;

    SymDataT testData1 = { std::byte{0b10000000}, std::byte{0b00000000} };
    SymDataT testData2 = { std::byte{0b00001111}, std::byte{0b11111111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto comp = SymT::Order();

    ASSERT_FALSE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsInMap) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = garchiever::BytesSymbol<1>;
    using MapToInt64 = std::map<SymT, std::int64_t, SymT::Order>;

    SymDataT testData1 = { std::byte{0b00000000} };
    SymDataT testData2 = { std::byte{0b11111111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto m = MapToInt64();

    m[sym1] = 37;
    m[sym2] = 42;

    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m[sym1], 37);
    EXPECT_EQ(m.lower_bound(sym1)->second, 37);
    EXPECT_EQ(m.upper_bound(sym1)->second, 42);
}
