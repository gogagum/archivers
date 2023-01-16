#include <gtest/gtest.h>

#include "../include/word/integer_word.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(IntegerWord, Construct) {
    [[maybe_unused]] auto word = ga::w::IntegerWord<int, 0, 8>(42);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, IncorrectOrdConstruct) {
    using TestedWord = ga::w::IntegerWord<int, 0, 8>;
    EXPECT_THROW(auto word = TestedWord(260),
                 TestedWord::IncorrectOrd);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Eq1) {
    using TestedWord = ga::w::IntegerWord<int, 0, 16>;
    auto w1 = TestedWord(34);
    auto w2 = TestedWord(34);
    EXPECT_TRUE(w1 == w2);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Eq2) {
    using TestedWord = ga::w::IntegerWord<int, 0, 16>;
    auto w1 = TestedWord(34);
    auto w2 = TestedWord(23);
    EXPECT_FALSE(w1 == w2);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Neq) {
    using TestedWord = ga::w::IntegerWord<int, 0, 7>;
    auto w1 = TestedWord(34);
    auto w2 = TestedWord(23);
    EXPECT_TRUE(w1 != w2);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, Ord) {
    using TestedWord = ga::w::IntegerWord<int, 37, 7>;
    auto w1 = TestedWord(42);
    EXPECT_EQ(TestedWord::ord(w1), 5);
}

//----------------------------------------------------------------------------//
TEST(IntegerWord, ByOrd) {
    using TestedWord = ga::w::IntegerWord<int, 37, 6>;
    auto w1 = TestedWord(42);
    EXPECT_EQ(TestedWord::byOrd(5), w1);
}
