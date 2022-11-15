#include <iostream>

#include "include/arithmetic_coder.hpp"
#include "include/byte_flow.hpp"

int main()
{
    std::array<std::byte, 64> data;
    auto byteFlow = garchiever::ByteFlow<garchiever::BytesSymbol<1>>(data.data(), 64);
    auto coder = garchiever::ArithmeticCoder(std::move(byteFlow));
    auto encoded = coder.encode();

    return 0;
}
