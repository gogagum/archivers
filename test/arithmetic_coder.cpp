#include <gtest/gtest.h>

#include "arithmetic_coder.hpp"
#include "flow/bits_word_flow.hpp"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
TEST(ArithmeticCoder, LambdaConstruct0) {
    using Word = ga::w::BitsWord<3>;
    auto dataBytes = std::array<std::byte, 32>{};
    auto flow = ga::fl::BitsWordFlow<3>(dataBytes);
    [[maybe_unused]] auto coder =
            ga::ArithmeticCoder<ga::fl::BitsWordFlow<3>>(flow);
}

//----------------------------------------------------------------------------//
TEST(ArithmeticCoder, LambdaConstruct2) {
    using Word = ga::w::BitsWord<31>;
    auto dataBytes = std::array<std::byte, 32>{};
    auto flow = ga::fl::BitsWordFlow<31>(dataBytes);
    [[maybe_unused]] auto coder =
            ga::ArithmeticCoder<ga::fl::BitsWordFlow<31>>(flow);
}

