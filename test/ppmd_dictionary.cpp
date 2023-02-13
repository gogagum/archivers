#include <gtest/gtest.h>

#include <dictionary/ppmd_dictionary.hpp>
#include <word/bytes_word.hpp>

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(PPMDDictionary, Construct) {
    const auto dict = ga::dict::PPMDDictionary<ga::w::BytesWord<1>, 3>();
}

