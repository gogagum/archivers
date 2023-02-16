#include <gtest/gtest.h>

#include <ael/word/integer_word.hpp>

using ga::w::IntegerWord;

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(IntegerWord, Construct) {
    [[maybe_unused]] auto word = IntegerWord<int, 0, 8>(42);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, IncorrectOrdConstruct) {
    using TestedWord = IntegerWord<int, 0, 8>;
    EXPECT_THROW(auto word = TestedWord(260), TestedWord::IncorrectOrd);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Eq1) {
    const auto w1 = IntegerWord<int, 0, 16>(34);
    const auto w2 = IntegerWord<int, 0, 16>(34);
    EXPECT_TRUE(w1 == w2);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Eq2) {
    const auto w1 = IntegerWord<int, 0, 16>(34);
    const auto w2 = IntegerWord<int, 0, 16>(23);
    EXPECT_FALSE(w1 == w2);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Neq) {
    const auto w1 = IntegerWord<int, 0, 7>(34);
    const auto w2 = IntegerWord<int, 0, 7>(23);
    EXPECT_TRUE(w1 != w2);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Ord) {
    using TestedWord = IntegerWord<int, 37, 7>;
    const auto w1 = TestedWord(42);
    EXPECT_EQ(TestedWord::ord(w1), 5);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, ByOrd) {
    using TestedWord = IntegerWord<int, 37, 6>;
    const auto w1 = TestedWord(42);
    EXPECT_EQ(TestedWord::byOrd(5), w1);
}
