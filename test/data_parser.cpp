#include <gtest/gtest.h>
#include <boost/range/irange.hpp>

#include "../include/data_parser.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DataParser, Construct) {
    std::vector<std::byte> testData{
        static_cast<std::byte>(std::uint8_t{2}), std::byte{25},
        std::byte{17}, std::byte{11} };
    [[maybe_unused]] auto decoded = ga::DataParser(testData);
}

//----------------------------------------------------------------------------//
TEST(DataParser, NumBytes) {
    std::vector<std::byte> testData{
        static_cast<std::byte>(std::uint8_t{2}), std::byte{25},
        std::byte{17}, std::byte{11} };

    auto decoded = ga::DataParser(testData);

    EXPECT_EQ(decoded.takeT<std::uint8_t>(), 2);
    EXPECT_EQ(decoded.takeByte(), std::byte(25));
    EXPECT_EQ(decoded.takeByte(), std::byte(17));
}

//----------------------------------------------------------------------------//
TEST(DataParser, TakeBit) {
    std::vector<std::byte> testData{ std::byte{0b10100110} };
                                               //|------|

    auto decoded = ga::DataParser(testData);

    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, TakeBits) {
    std::vector<std::byte> testData =
        { std::byte{0b10100110}, std::byte{0b00110110} };
                    //|------|             //|------|

    auto decoded = ga::DataParser(testData);

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

    auto decoded = ga::DataParser(testData);

    EXPECT_EQ(decoded.takeByte(), std::byte(2));
    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, BitAfterByte2) {
    std::vector<std::byte> testData{
        std::byte{2}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(testData);

    EXPECT_EQ(decoded.takeByte(), std::byte(2));
    EXPECT_FALSE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, ByteAfterBit0) {
    std::vector<std::byte> testData{
        std::byte{0b00000010}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(testData);

    decoded.takeBit();
    EXPECT_EQ(decoded.takeByte(), std::byte{0b00000100});
}

//----------------------------------------------------------------------------//
TEST(DataParser, ByteAfterBit1) {
    std::vector<std::byte> testData{
        std::byte{0b00000010}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(testData);

    for (auto _: boost::irange(0, 8)) {
        decoded.takeBit();
    }

    EXPECT_EQ(decoded.takeByte(), testData[1]);
}

//----------------------------------------------------------------------------//
TEST(DataParser, UInt16T) {
    std::vector<std::byte> testData{
        std::byte{0b01100010}, std::byte{0b00100110} };

    auto decoded = ga::DataParser(testData);

    EXPECT_EQ(decoded.takeT<std::uint16_t>(),
              std::uint16_t{0b0110001000100110});
}

//----------------------------------------------------------------------------//
TEST(DataParser, Seek0) {
    std::vector<std::byte> testData{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    auto parser = ga::DataParser(testData);

    parser.takeBit();
    parser.takeBit();
    parser.takeBit();
    parser.seek(0);
    EXPECT_FALSE(parser.takeBit());
    EXPECT_TRUE(parser.takeBit());
    EXPECT_FALSE(parser.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, Seek1) {
    std::vector<std::byte> testData{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    auto parser = ga::DataParser(testData);

    parser.seek(4);
    EXPECT_TRUE(parser.takeBit());
    EXPECT_TRUE(parser.takeBit());
    EXPECT_FALSE(parser.takeBit());
    EXPECT_FALSE(parser.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, Seek2) {
    std::vector<std::byte> testData{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    auto parser = ga::DataParser(testData);

    parser.seek(10);
    EXPECT_TRUE(parser.takeBit());
    EXPECT_TRUE(parser.takeBit());
    EXPECT_TRUE(parser.takeBit());
    EXPECT_TRUE(parser.takeBit());
    EXPECT_FALSE(parser.takeBit());
    EXPECT_FALSE(parser.takeBit());
}

//----------------------------------------------------------------------------//
TEST(DataParser, EqCompare0) {
    std::vector<std::byte> testData{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    auto p0 = ga::DataParser(testData);
    auto p1 = ga::DataParser(testData);

    EXPECT_EQ(p0, p1);
}

//----------------------------------------------------------------------------//
TEST(DataParser, EqCompare1) {
    std::vector<std::byte> testData0{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    std::vector<std::byte> testData1{
        std::byte{0b01001100}, std::byte{0b00100100}
    };

    auto p0 = ga::DataParser(testData0);
    auto p1 = ga::DataParser(testData1);

    EXPECT_NE(p0, p1);
}

//----------------------------------------------------------------------------//
TEST(DataParser, EqCompare2) {
    std::vector<std::byte> testData0{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    std::vector<std::byte> testData1{
        std::byte{0b01001100}, std::byte{0b00111100}
    };

    EXPECT_EQ(testData0, testData1);

    auto p0 = ga::DataParser(testData0);
    auto p1 = ga::DataParser(testData1);

    EXPECT_NE(p0, p1);
}
