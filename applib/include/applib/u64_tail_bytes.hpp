#ifndef U64_TAIL_BYTES_HPP
#define U64_TAIL_BYTES_HPP

#include <climits>
#include <cstdint>
#include <ranges>
#include <cassert>

static auto u64_tail_bytes(std::uint64_t i, std::size_t bytesCnt) {
  assert(bytesCnt <= sizeof(std::uint64_t));

  using std::views::iota;
  using std::views::reverse;
  using std::views::transform;

  constexpr auto mask = std::uint64_t{0xFF};

  const auto bytesIndicies = iota(0uz, bytesCnt) | reverse;
  return bytesIndicies | transform([i](std::size_t byteIdx) {
           return static_cast<std::byte>((i >> (byteIdx * CHAR_BIT)) & mask);
         });
}

#endif  // U64_TAIL_BYTES_HPP
