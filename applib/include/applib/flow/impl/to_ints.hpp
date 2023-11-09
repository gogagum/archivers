#ifndef TO_INTS_HPP
#define TO_INTS_HPP

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <ranges>

using std::views::chunk;
using std::views::iota;
using std::views::reverse;
using std::views::slide;
using std::views::stride;
using std::views::take;
using std::views::transform;
using std::views::zip;

template <std::integral I, std::size_t numBits>
  requires(sizeof(I) * 8 >= numBits)
struct ToInts {
  template <std::ranges::range Rng, std::size_t srcBits = 1>
    requires std::integral<std::ranges::range_value_t<Rng>>
  static auto perform(Rng &&rng, std::integral_constant<std::size_t, srcBits> = {}) {
    const auto sz = rng.size();
    const auto cnt = sz / numBits;
    return std::forward<Rng>(rng) | slide(numBits) | stride(numBits) |
           transform([](const auto &bitsRng) {
             constexpr static auto digits =
                 iota(0uz, numBits) | reverse | transform([](std::size_t i) {
                   return I{1} << static_cast<I>(i * srcBits);
                 });
             const auto prods =
                 zip(bitsRng, digits) | transform([](const auto &pair) -> I {
                   const auto &[bit, digit] = pair;
                   return I{bit} * digit;
                 });
             return std::ranges::fold_left(prods, I{0},
                                           [](I l, I r) { return l + r; });
           });
  }
};

#endif
