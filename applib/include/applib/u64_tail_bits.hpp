#ifndef U64_TAIL_BITS_HPP
#define U64_TAIL_BITS_HPP

#include <cstdint>
#include <climits>
#include <cassert>
#include <ranges>

static auto u64_tail_bits(std::uint64_t i, std::size_t numBits) {
  assert(numBits <= sizeof(std::uint64_t) * CHAR_BIT);

  using std::views::iota;
  using std::views::reverse;
  using std::views::transform;

  const auto bytesIndicies = iota(0uz, numBits) | reverse;
  return bytesIndicies | transform([i](std::size_t bitIdx) {
           return static_cast<bool>((i >> bitIdx) & std::uint64_t{1});
         });
}

#endif  // U64_TAIL_BITS_HPP
