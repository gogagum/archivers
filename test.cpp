#include <gtest/gtest.h>

#include "include/byte_data_constructor.hpp"
#include "include/arithmetic_decoder_decoded.hpp"
#include "include/word/bytes_symbol.hpp"
#include "include/word/int_range_word.hpp"
#include "include/word/word_ord_comp.hpp"
#include "include/dictionary/uniform_dictionary.hpp"
#include "include/dictionary/static_dictionary.hpp"
#include "include/dictionary/adaptive_dictionary.hpp"
#include "include/flow/int_range_word_flow.hpp"
#include "include/misc.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsIterator, Construct) {
    std::uint64_t num;
    auto iter = ga::impl::BitsIterator(num, 3);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref) {
    std::uint64_t num = 0;
    auto iter = ga::impl::BitsIterator(num, 3);
    EXPECT_FALSE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref2) {
    auto num = std::byte{0b11110000};
    auto iter = ga::impl::BitsIterator(num, 2);
    EXPECT_TRUE(*iter);
}

//----------------------------------------------------------------------------//
TEST(BitsIterator, Deref3) {
    auto num = std::byte{0b11110000};
    auto iter = ga::impl::BitsIterator(num, 7);
    EXPECT_FALSE(*iter);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, Construct) {
    auto num = 42;
    ga::impl::make_bits_iterator_range(num);
}

//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, Size) {
    std::uint32_t num = 42;
    auto rng = ga::impl::make_bits_iterator_range(num);
    EXPECT_EQ(std::size(rng), 32);
}

//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, Iterate) {
    std::uint32_t num = 42;
    std::size_t cnt = 0;
    for ([[maybe_unused]] bool bit : ga::impl::make_bits_iterator_range(num)) {
        ++cnt;
    }
    EXPECT_EQ(cnt, 32);
}

//----------------------------------------------------------------------------//
TEST(BitsIteratorRange, BoolVec) {
    std::uint32_t num = 0xFF00A5C3;  // 1111 1111 0000 0000 1100 0101 1010 0011
    auto bitsRng = ga::impl::make_bits_iterator_range(num);
    auto vec = std::vector(bitsRng.begin(), bitsRng.end());
    EXPECT_EQ(vec[4], true);
    EXPECT_EQ(vec[7], true);
    EXPECT_EQ(vec[9], false);
    EXPECT_EQ(vec[12], false);
    EXPECT_EQ(vec[16], true);
}


////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, Consruct) {
    auto encoded = ga::ByteDataConstructor();
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitFalse) {
    auto encoded = ga::ByteDataConstructor();
    encoded.putBit(false);
    EXPECT_EQ(encoded.data()[0], std::byte{0b00000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitTrue) {
    auto encoded = ga::ByteDataConstructor();
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBits) {
    auto encoded = ga::ByteDataConstructor();
    encoded.putBit(true);
    encoded.putBit(false);
    encoded.putBit(false);
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{0b10010000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBits2) {
    auto encoded = ga::ByteDataConstructor();
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
    auto encoded = ga::ByteDataConstructor();
    encoded.putByte(std::byte{42});
    EXPECT_EQ(encoded.data()[0], std::byte{42});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutBitAfterByte) {
    auto encoded = ga::ByteDataConstructor();
    encoded.putByte(std::byte{42});
    encoded.putBit(true);
    EXPECT_EQ(encoded.data()[0], std::byte{42});
    EXPECT_EQ(encoded.data()[1], std::byte{0b10000000});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutByte2) {
    auto encoded = ga::ByteDataConstructor();
    encoded.putByte(std::byte{42});
    encoded.putByte(std::byte{37});
    encoded.putByte(std::byte{73});
    EXPECT_EQ(encoded.data()[0], std::byte{42});
    EXPECT_EQ(encoded.data()[1], std::byte{37});
    EXPECT_EQ(encoded.data()[2], std::byte{73});
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoderEncodedTest, PutInt64) {
    auto encoded = ga::ByteDataConstructor();
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
    auto encoded = ga::ByteDataConstructor();
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
    [[maybe_unused]] auto sym = ga::w::BytesSymbol<5>(testData.data());
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, ConstructFromArray) {
    std::array<std::byte, 5> testData;
    [[maybe_unused]] auto sym = ga::w::BytesSymbol<5>(testData);
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder1) {
    using SymDataT = std::array<std::byte, 2>;
    using SymT = ga::w::BytesSymbol<2>;

    SymDataT testData1 = { std::byte{0b00000000}, std::byte{0b11111111} };
    SymDataT testData2 = { std::byte{0b11111111}, std::byte{0b11111111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto comp = ga::w::WordOrdComp<SymT>();

    ASSERT_TRUE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder2) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = ga::w::BytesSymbol<1>;

    SymDataT testData1 = { std::byte{0b00000111} };
    SymDataT testData2 = { std::byte{0b00001111} };

    auto sym1 = SymT(testData1.data());
    auto sym2 = SymT(testData2.data());

    auto comp = ga::w::WordOrdComp<SymT>();

    ASSERT_TRUE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder3) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = ga::w::BytesSymbol<1>;

    SymDataT testData1 = { std::byte{0b00001111} };
    SymDataT testData2 = { std::byte{0b00000111} };

    auto sym1 = SymT(testData1);
    auto sym2 = SymT(testData2);

    auto comp = ga::w::WordOrdComp<SymT>();

    ASSERT_FALSE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsOrder4) {
    using SymDataT = std::array<std::byte, 2>;
    using SymT = ga::w::BytesSymbol<2>;

    SymDataT testData1 = { std::byte{0b10000000}, std::byte{0b00000000} };
    SymDataT testData2 = { std::byte{0b00001111}, std::byte{0b11111111} };

    auto sym1 = SymT(testData1);
    auto sym2 = SymT(testData2);

    auto comp = ga::w::WordOrdComp<SymT>();

    ASSERT_FALSE(comp(sym1, sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesSymbolTest, BytesSymbolsInMap) {
    using SymDataT = std::array<std::byte, 1>;
    using SymT = ga::w::BytesSymbol<1>;
    using MapToInt64 = std::map<SymT, std::int64_t, ga::w::WordOrdComp<SymT>>;

    SymDataT testData1 = { std::byte{0b00000000} };
    SymDataT testData2 = { std::byte{0b11111111} };

    auto sym1 = SymT(testData1);
    auto sym2 = SymT(testData2);

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
    using SymT = ga::w::BytesSymbol<1>;
    using MapToInt64 = std::map<SymT, std::int64_t, ga::w::WordOrdComp<SymT>>;

    SymDataT testData1 = { std::byte{0b00000000} };
    SymDataT testData2 = { std::byte{0b00001111} };
    SymDataT testData3 = { std::byte{0b11111111} };

    auto sym1 = SymT(testData1);
    auto sym2 = SymT(testData2);
    auto sym3 = SymT(testData3);

    auto m = MapToInt64();

    m[sym1] = 37;
    m[sym3] = 42;

    EXPECT_EQ(m.size(), 2);

    EXPECT_EQ(m.lower_bound(sym2)->second, 42);
    EXPECT_EQ(m.upper_bound(sym2)->second, 42);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(IntRangeWord, Construct) {
    [[maybe_unused]] auto word = ga::w::IntegerWord<int, 0, 8>(42);
}

//----------------------------------------------------------------------------//
TEST(IntRangeWord, IncorrectOrdConstruct) {
    using TestedWord = ga::w::IntegerWord<int, 0, 8>;
    EXPECT_THROW(auto word = TestedWord(260),
                 TestedWord::IncorrectOrd);
}

//----------------------------------------------------------------------------//
TEST(IntRangeWord, Eq1) {
    using TestedWord = ga::w::IntegerWord<int, 0, 16>;
    auto w1 = TestedWord(34);
    auto w2 = TestedWord(34);
    EXPECT_TRUE(w1 == w2);
}

//----------------------------------------------------------------------------//
TEST(IntRangeWord, Eq2) {
    using TestedWord = ga::w::IntegerWord<int, 0, 16>;
    auto w1 = TestedWord(34);
    auto w2 = TestedWord(23);
    EXPECT_FALSE(w1 == w2);
}

//----------------------------------------------------------------------------//
TEST(IntRangeWord, Neq) {
    using TestedWord = ga::w::IntegerWord<int, 0, 7>;
    auto w1 = TestedWord(34);
    auto w2 = TestedWord(23);
    EXPECT_TRUE(w1 != w2);
}

//----------------------------------------------------------------------------//
TEST(IntRangeWord, Ord) {
    using TestedWord = ga::w::IntegerWord<int, 37, 7>;
    auto w1 = TestedWord(42);
    EXPECT_EQ(TestedWord::ord(w1), 5);
}

//----------------------------------------------------------------------------//
TEST(IntRangeWord, ByOrd) {
    using TestedWord = ga::w::IntegerWord<int, 37, 6>;
    auto w1 = TestedWord(42);
    EXPECT_EQ(TestedWord::byOrd(5), w1);
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
        std::byte{0b00000010}, std::byte{0b00100110} };

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
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<2>>();
}

//----------------------------------------------------------------------------//
TEST(UniformDict, Ord2Bytes) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<2>>();
    auto symBytes = std::array{std::byte{0x00}, std::byte{0x02}};
    EXPECT_EQ(dict.getWord(2), ga::w::BytesSymbol<2>(symBytes));
}

//----------------------------------------------------------------------------//
TEST(UniformDict, Ord3Bytes) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<3>>();
    auto symBytes =
        std::array{std::byte{0x00}, std::byte{0x00}, std::byte{0x1A}};
    EXPECT_EQ(dict.getWord(26), ga::w::BytesSymbol<3>(symBytes));
}

//----------------------------------------------------------------------------//
TEST(UniformDict, OrdLongOrd) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<2>>();
    auto symBytes = std::array{ std::byte{8}, std::byte{42} };
    EXPECT_EQ(dict.getWord(256 * 8 + 42), ga::w::BytesSymbol<2>(symBytes));
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundLow) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{37}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 37);
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundLowZero) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{0}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundHigh) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{37}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 38);
}

//----------------------------------------------------------------------------//
TEST(UniformDict, CumulativeNumFoundHighZero) {
    auto dict = ga::dict::UniformDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{0}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 1);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(StaticDictionary, Construct) {
    auto freq = std::array<std::uint64_t, 256>{};
    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<1>>(freq);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes1) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = ga::w::BytesSymbol<2>(word1Bytes);

    EXPECT_EQ(dict.getWord(37), word1);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, Ord2Bytes2) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto wordBytes = std::array{std::byte{0}, std::byte{42}};
    auto word = ga::w::BytesSymbol<2>(wordBytes);

    EXPECT_EQ(dict.getWord(36), word);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundLow) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word1 = ga::w::BytesSymbol<2>(word1Bytes);

    auto word2Bytes = std::array<std::byte, 2>{std::byte{0}, std::byte{113}};
    auto word2 = ga::w::BytesSymbol<2>(word2Bytes);

    EXPECT_EQ(dict.getLowerCumulativeNumFound(word1), 37);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word2), 42);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundZero) {
    auto freq = std::vector<std::uint64_t>(256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<1>>(freq);

    auto wordBytes = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesSymbol<1>(wordBytes);

    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(StaticDictionary, CumulativeNumFoundHigh) {
    auto freq = std::vector<std::uint64_t>(256 * 256, 0);
    std::fill(freq.begin() + 42, freq.begin() + 112, 37);
    std::fill(freq.begin() + 112, freq.end(), 42);

    auto dict = ga::dict::StaticDictionary<ga::w::BytesSymbol<2>>(freq);

    auto word1Bytes = std::array{std::byte{0}, std::byte{111}};
    auto word1 = ga::w::BytesSymbol<2>(word1Bytes);

    auto word2Bytes = std::array{std::byte{0}, std::byte{112}};
    auto word2 = ga::w::BytesSymbol<2>(word2Bytes);

    EXPECT_EQ(dict.getHigherCumulativeNumFound(word1), 37);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word2), 42);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(DynamicDictionary, Construct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<2>>();
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, LowerAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{37}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 37);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, LowerZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array<std::byte, 1>{std::byte{0}};
    auto word = ga::w::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 0);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, HigherZeroAfterConstruct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesSymbol<1>(symData);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, Increase) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto increasedWordData = std::array{std::byte{12}};
    dict.increaseWordCount(ga::w::BytesSymbol<1>(increasedWordData));
    auto symData = std::array{std::byte{42}};
    auto word = ga::w::BytesSymbol<1>(symData.data());
    EXPECT_EQ(dict.getLowerCumulativeNumFound(word), 43);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, IncreaseExactTheSame) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto increasedWordData = std::array{std::byte{12}};
    auto increasedWord = ga::w::BytesSymbol<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 12);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 14);
}

//----------------------------------------------------------------------------//
TEST(DynamicDictionary, IncreaseZero) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesSymbol<1>>();
    auto increasedWordData = std::array{std::byte{0}};
    auto increasedWord = ga::w::BytesSymbol<1>(increasedWordData);
    dict.increaseWordCount(increasedWord);
    EXPECT_EQ(dict.getLowerCumulativeNumFound(increasedWord), 0);
    EXPECT_EQ(dict.getHigherCumulativeNumFound(increasedWord), 2);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(IntRangeFlow, construct) {
    auto vec = std::vector<int>{3, 7, 2};
    auto flow = ga::fl::IntegerWordFlow<int, -5, 8>(std::move(vec));
}

//----------------------------------------------------------------------------//
TEST(IntRangeFlow, numWords) {
    auto vec = std::vector<int>{3, 7, 2};
    auto flow = ga::fl::IntegerWordFlow<int, -5, 8>(std::move(vec));
    EXPECT_EQ(flow.getNumberOfWords(), 3);
}

//----------------------------------------------------------------------------//
TEST(IntRangeFlow, iterate) {
    auto vec = std::vector<int>{3, 7, 2};
    auto flow = ga::fl::IntegerWordFlow<int, -5, 4>(std::move(vec));

    for (auto word: flow) {

    }
}
