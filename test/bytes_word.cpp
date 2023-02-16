#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <vector>
#include <boost/range/combine.hpp>

#include <ael/word/bytes_word.hpp>

using ael::w::BytesWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(BytesWord, Construct) {
    const auto testData = std::array<std::byte, 5>{};
    [[maybe_unused]] const auto sym = BytesWord<5>(testData.data());
}

//----------------------------------------------------------------------------//
TEST(BytesWord, ConstructFromArray) {
    const auto testData = std::array<std::byte, 5>{};
    [[maybe_unused]] const auto sym = BytesWord<5>(testData.begin());
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder1) {
    const auto testData1 =
            std::array{ std::byte{0b00000000}, std::byte{0b11111111} };
    const auto testData2 =
            std::array{ std::byte{0b11111111}, std::byte{0b11111111} };

    const auto sym1 = BytesWord<2>(testData1.data());
    const auto sym2 = BytesWord<2>(testData2.data());

    ASSERT_TRUE(BytesWord<2>::ord(sym1) < BytesWord<2>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder2) {
    const auto sym1 = BytesWord<1>(std::array{ std::byte{0b00000111} }.begin());
    const auto sym2 = BytesWord<1>(std::array{ std::byte{0b00001111} }.begin());

    ASSERT_TRUE(BytesWord<1>::ord(sym1) < BytesWord<1>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder3) {
    const auto sym1 = BytesWord<1>(std::array{ std::byte{0b00001111} }.begin());
    const auto sym2 = BytesWord<1>(std::array{ std::byte{0b00000111} }.begin());

    ASSERT_FALSE(BytesWord<1>::ord(sym1) < BytesWord<1>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesSymbolsOrder4) {
    const auto testData1 = std::array{ std::byte{0b10000000}, std::byte{0b00000000} };
    const auto testData2 = std::array{ std::byte{0b00001111}, std::byte{0b11111111} };

    const auto sym1 = BytesWord<2>(testData1.begin());
    const auto sym2 = BytesWord<2>(testData2.begin());

    ASSERT_FALSE(BytesWord<2>::ord(sym1) < BytesWord<2>::ord(sym2));
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesOut) {
    const auto testData = std::array{ std::byte{0b00001111} };
    const auto word = BytesWord<1>(testData.begin());

    std::vector<std::byte> bytes;
    auto bytesInserter = std::back_inserter(bytes);

    word.bytesOut(bytesInserter);
    EXPECT_EQ(bytes.size(), 1);

    EXPECT_EQ(bytes[0], testData[0]);
}

//----------------------------------------------------------------------------//
TEST(BytesWord, BytesOutThreeBytes) {
    const auto testData = std::array{ std::byte{0b00001111},
                                      std::byte{0b11000101},
                                      std::byte{0b11011001} };
    const auto word = BytesWord<3>(testData.begin());

    std::vector<std::byte> bytes;
    word.bytesOut(std::back_inserter(bytes));
    EXPECT_EQ(bytes.size(), 3);

    for (const auto& [byteI, testByteI] : boost::range::combine(bytes,
                                                                testData)) {
        EXPECT_EQ(byteI, testByteI);
    }
}

//----------------------------------------------------------------------------//
TEST(BytesWord, FromBytesOut) {
    const auto testData = std::array{ std::byte{0b00001111},
                                      std::byte{0b11000101},
                                      std::byte{0b11011001} };
    const auto word = BytesWord<3>(testData.begin());

    std::vector<std::byte> bytes;
    word.bytesOut(std::back_inserter(bytes));

    const auto otherWord = BytesWord<3>(bytes.data());

    EXPECT_EQ(word, otherWord);
}

//----------------------------------------------------------------------------//
TEST(BytesWord, ByOrdOrd) {
    const auto word = BytesWord<2>(
        std::array{std::byte{0b10001110}, std::byte{0b10101010}}.begin());

    const auto ord = BytesWord<2>::ord(word);

    EXPECT_EQ(BytesWord<2>::byOrd(ord), word);
}

//----------------------------------------------------------------------------//
TEST(BytesWord, OrdByOrd) {
    const auto ord = 42;
    const auto word = BytesWord<2>::byOrd(ord);

    EXPECT_EQ(BytesWord<2>::ord(word), ord);
}

//----------------------------------------------------------------------------//
TEST(BytesWord, Output) {
    const auto testData = std::array{ std::byte{0b00101101} };
    const auto word = BytesWord<1>(testData.begin());

    auto testOutStream = std::ostringstream();
    testOutStream << word;

    EXPECT_EQ(testOutStream.str(), "00101101");
}
