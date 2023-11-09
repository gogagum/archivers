#ifndef TO_BITS_HPP
#define TO_BITS_HPP

#include <ranges>
#include <cstdint>

using std::views::cartesian_product;
using std::views::iota;
using std::views::join;
using std::views::reverse;
using std::views::transform;

template <class RetT = std::byte> struct ToBits;

template <> struct ToBits<std::byte> {
  ToBits() = delete;

  template <std::ranges::range Rng> static auto perform(Rng &&rng) {
    using T = std::remove_cvref_t<std::ranges::range_value_t<Rng>>;
    constexpr auto bitsIndicies = iota(0uz, 8 * sizeof(T)) | reverse;
    return cartesian_product(std::forward<Rng>(rng), bitsIndicies) |
           transform([](const auto &pair) -> std::byte {
             auto &[b, i] = pair;
             return static_cast<std::byte>((b >> i) & T{1});
           });
  }
};

template <std::integral I> struct ToBits<I> {
  ToBits() = delete;
  template <std::ranges::range Rng>
  static auto perform(Rng &&rng)
  {
    using T = std::remove_cvref_t<std::ranges::range_value_t<Rng>>;
    constexpr auto bitsIndicies = iota(0uz, 8 * sizeof(T)) | reverse;
    return cartesian_product(std::forward<Rng>(rng), bitsIndicies) |
           transform([](const auto &pair) -> I {
             auto &[b, i] = pair;
             if constexpr (std::is_same_v<T, std::byte>) {
              return std::to_integer<I>(b >> i) & I{1};
             } else {
               return (b >> i) & I{1};
             }
           });
  }
};

#endif
