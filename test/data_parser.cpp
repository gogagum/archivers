#include <gtest/gtest.h>

#include "../include/data_parser.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DataParser, Construct) {
    std::vector<std::byte> testData{
        static_cast<std::byte>(std::uint8_t{2}), std::byte{25},
        std::byte{17}, std::byte{11} };
    auto decoded = ga::DataParser(std::move(testData));
}

//----------------------------------------------------------------------------//
TEST(DataParser, NumBytes) {
    std::vector<std::byte> testData{
        static_cast<std::byte>(std::uint8_t{2}), std::byte{25},
        std::byte{17}, std::byte{11} };

    auto decoded = ga::DataParser(std::move(testData));

    EXPECT_EQ(decoded.takeT<std::uint8_t>(), 2);
    EXPECT_EQ(decoded.takeByte(), std::byte(25));
    EXPECT_EQ(decoded.takeByte(), std::byte(17));
}


//----------------------------------------------------------------------------//
TEST(DataParser, TakeBit) {
    std::vector<std::byte> testData{ std::byte{0b10100110} };
                                               //|------|

    auto decoded = ga::DataParser(std::move(testData));

    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, TakeBits) {
    std::vector<std::byte> testData =
        { std::byte{0b10100110}, std::byte{0b00110110} };
                    //|------|             //|------|

    auto decoded = ga::DataParser(std::move(testData));

    EXPECT_TRUE(decoded.takeBit());
    EXPECT_FALSE(decoded.takeBit());
    EXPECT_TRUE(decoded.takeBit());
    EXPECT_FALSE(decoded.takeBit());
    EXPECT_FALSE(decoded.takeBit());
    EXPECT_TRUE(decoded.takeBit());
    EXPECT_TRUE(decoded.takeBit());
    EXPECT_FALSE(decoded.takeBit());
    EXPECT_FALSE(decoded.takeBit());
    EXPECT_FALSE(decoded.takeBit());
    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, BitAfterByte1) {
    std::vector<std::byte> testData{
        std::byte{2}, std::byte{0b10100110} };

    auto decoded = ga::DataParser(std::move(testData));

    EXPECT_EQ(decoded.takeByte(), std::byte(2));
    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, BitAfterByte2) {
    std::vector<std::byte> testData{
        std::byte{2}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(std::move(testData));

    EXPECT_EQ(decoded.takeByte(), std::byte(2));
    EXPECT_FALSE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, ByteAfterBit) {
    std::vector<std::byte> testData{
        std::byte{0b00000010}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(std::move(testData));

    decoded.takeBit();
    EXPECT_THROW(decoded.takeByte(),
                 ga::DataParser::BytesAfterBitsException);
}

//----------------------------------------------------------------------------//
TEST(DataParser, UInt16T) {
    std::vector<std::byte> testData{
        std::byte{0b01100010}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(std::move(testData));

    EXPECT_EQ(decoded.takeT<std::uint16_t>(),
              std::uint16_t{0b0110001000100110});
}
