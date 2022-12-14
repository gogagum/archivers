#include <gtest/gtest.h>

#include "include/arithmetic_coder_encoded.hpp"
#include "include/arithmetic_decoder_decoded.hpp"
#include "include/bytes_symbol.hpp"
#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/static_dicitionary.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, Consruct) {
    auto encoded = ga::ArithmeticCoderEncoded();
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitFalse) {
    auto encoded = ga::ArithmeticCoderEncoded();
    encoded.putBit(false);
    EXPECT_EQ(encoded.data()[0], std::byte{0b00000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitTrue) {
    auto encoded = ga::ArithmeticCoderEncoded();
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBits) {
    auto encoded = ga::ArithmeticCoderEncoded();
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(false);
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10010000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBits2) {
    auto encoded = ga::ArithmeticCoderEncoded();
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
    auto encoded = ga::ArithmeticCoderEncoded();
    encoded.putByte(std::byte{42});
    EXPECT_EQ(encoded.data()[0], std::byte{42});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitAfterByte) {
    auto encoded = ga::ArithmeticCoderEncoded();
    encoded.putByte(std::byte{42});
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{42});
    EXPECT_EQ(encoded.data()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutByte2) {
    auto encoded = ga::ArithmeticCoderEncoded();
    encoded.putByte(std::byte{42});
    encoded.putByte(std::byte{37});
    encoded.putByte(std::byte{73});
    EXPECT_EQ(encoded.data()[0], std::byte{42});
    EXPECT_EQ(encoded.data()[1], std::byte{37});
    EXPECT_EQ(encoded.data()[2], std::byte{73});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutInt64) {
    auto encoded = ga::ArithmeticCoderEncoded();
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
    auto encoded = ga::ArithmeticCoderEncoded();
    std::uint32_t tested = 0b00000000001000001000000000001000;
                           //00000000111111112222222233333333

    encoded.putT(tested);
    EXPECT_EQ(encoded.bytesSize(), 4);
    EXPECT_EQ(encoded.data()[0], std::byte(0b00000000));
    EXPECT_EQ(encoded.data()[1], std::byte{0b00100000});
    EXPECT_EQ(encoded.data()[2], std::byte{0b10000000});
    EXPECT_EQ(encoded.data()[3], std::byte{0b00001000});
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, Construct) {
    std::array<std::byte, 5> testData;
    [[maybe_unused]] auto sym = ga::BytesSymbol<5>(testData.data());
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder1) {
    using SymDataT = std::array<std::byte, 2>;
    using SymT = ga::BytesSymbol<2>;

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
    using SymT = ga::BytesSymbol<1>;

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
    using SymT = ga::BytesSymbol<1>;

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
    using SymT = ga::BytesSymbol<2>;

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
    using SymT = ga::BytesSymbol<1>;
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

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsInMapUpperBoundOfSymNotInMap) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = ga::BytesSymbol<1>;
    using MapToInt64 = std::map<SymT, std::int64_t, SymT::Order>;

    SymDataT testData1 = { std::byte{0b00000000} };
    SymDataT testData2 = { std::byte{0b00001111} };
    SymDataT testData3 = { std::byte{0b11111111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());
    auto sym3 = SymT(testData3.data());

    auto m = MapToInt64();

    m[sym1] = 37;
    m[sym3] = 42;

    EXPECT_EQ(m.size(), 2);

    EXPECT_EQ(m.lower_bound(sym2)->second, 42);
    EXPECT_EQ(m.upper_bound(sym2)->second, 42);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, Construct) {
    std::vector<std::byte> testData{
        static_cast<std::byte>(std::uint8_t{2}), std::byte{25},
        std::byte{17}, std::byte{11} };
    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));
}

//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, NumBytes) {
    std::vector<std::byte> testData{
        static_cast<std::byte>(std::uint8_t{2}), std::byte{25},
        std::byte{17}, std::byte{11} };

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

    EXPECT_EQ(decoded.takeT<std::uint8_t>(), 2);
    EXPECT_EQ(decoded.takeByte(), std::byte(25));
    EXPECT_EQ(decoded.takeByte(), std::byte(17));
}


//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, TakeBit) {
    std::vector<std::byte> testData{ std::byte{0b10100110} };
                                               //|------|

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, TakeBits) {
    std::vector<std::byte> testData =
        { std::byte{0b10100110}, std::byte{0b00110110} };
                    //|------|             //|------|

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

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
TEST(ArithmeticDecoderDecoded, BitAfterByte1) {
    std::vector<std::byte> testData{
        std::byte{2}, std::byte{0b10100110} };

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

    EXPECT_EQ(decoded.takeByte(), std::byte(2));
    EXPECT_TRUE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, BitAfterByte2) {
    std::vector<std::byte> testData{
        std::byte{2}, std::byte{0b00100110} };

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

    EXPECT_EQ(decoded.takeByte(), std::byte(2));
    EXPECT_FALSE(decoded.takeBit());
}

//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, ByteAfterBit) {
    std::vector<std::byte> testData{
        std::byte{2}, std::byte{0b00100110} };

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

    decoded.takeBit();
    EXPECT_THROW(decoded.takeByte(),
                 ga::ArithmeticDecoderDecoded::BytesAfterBitsException);
}

//----------------------------------------------------------------------------//
TEST(ArithmeticDecoderDecoded, UInt16T) {
    std::vector<std::byte> testData{
        std::byte{0b01100010}, std::byte{0b00100110} };

    auto decoded = ga::ArithmeticDecoderDecoded(std::move(testData));

    EXPECT_EQ(decoded.takeT<std::uint16_t>(),
              std::uint16_t{0b0110001000100110});
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(UniformDict, Construct) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<2>>();
}

//----------------------------------------------------------------------------//
TEST(UniformDict, Ord2Bytes) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<2>>();
    std::array<std::byte, 2> symBytes {std::byte{0x00}, std::byte{0x02}};
    EXPECT_EQ(dict.getWord(2), ga::BytesSymbol<2>(symBytes.data()));
}

//----------------------------------------------------------------------------//
TEST(UniformDict, Ord3Bytes) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<3>>();
    std::array<std::byte, 3> symBytes =
        {std::byte{0x00}, std::byte{0x00}, std::byte{0x1A}};
    EXPECT_EQ(dict.getWord(26), ga::BytesSymbol<3>(symBytes.data()));
}

//----------------------------------------------------------------------------//
TEST(UniformDict, OrdLongOrd) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<2>>();
    std::array<std::byte, 2> symBytes = { std::byte{8}, std::byte{42} };
    EXPECT_EQ(dict.getWord(256 * 8 + 42), ga::BytesSymbol<2>(symBytes.data()));
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundLow) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{37}};
    auto word = ga::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 37);
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundLowZero) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundHigh) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{37}};
    auto word = ga::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 38);
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundHighZero) {
    auto dict = ga::dict::UniformDictionary<ga::BytesSymbol<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 1);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(StaticDictionary, Construct) {
    auto freq = std::array<std::uint64_t, 256>{};
    auto dict = ga::dict::StaticDictionary<ga::BytesSymbol<1>>(freq.begin(), freq.end());
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes1) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::BytesSymbol<2>>(freq.begin(), freq.end());

    auto word1Bytes = std::array<std::byte, 2>{std::byte{0}, std::byte{111}};
    auto word1 = ga::BytesSymbol<2>(word1Bytes.data());

    EXPECT_EQ(dict.getWord(37), word1);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes2) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::BytesSymbol<2>>(freq.begin(), freq.end());

    auto word1Bytes = std::array<std::byte, 2>{std::byte{0}, std::byte{41}};
    auto word1 = ga::BytesSymbol<2>(word1Bytes.data());

    EXPECT_EQ(dict.getWord(36), word1);
}
