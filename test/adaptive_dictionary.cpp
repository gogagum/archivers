#include <gtest/gtest.h>

#include "../include/dictionary/adaptive_dictionary.hpp"
#include "../include/word/bytes_word.hpp"
#include "../include/dictionary/traits.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
static_assert(ga::dict::traits::needWordIncrease<
                  ga::dict::AdaptiveDictionary<ga::w::BytesWord<2>>
              >);

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(AdaptiveDictionary, Construct) {
    auto dict = ga::dict::AdaptiveDictionary<ga::w::BytesWord<2>>();
}
