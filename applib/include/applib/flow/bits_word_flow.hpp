#ifndef BIT_WORD_FLOW_HPP
#define BIT_WORD_FLOW_HPP

#include <applib/flow/impl/to_bits.hpp>
#include <applib/flow/impl/to_ints.hpp>

#include <cstdint>
#include <ranges>
#include <span>

using std::views::drop;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BitsWordFlow class
///
template <std::uint16_t _numBits> class BitsWordFlow {
private:
  using BitsRng_ = decltype(ToBits<std::uint64_t>::perform(
      std::declval<const std::span<const std::byte> &>()));

  using IntsRng_ = decltype(ToInts<std::uint64_t, _numBits>::perform(
      std::declval<const BitsRng_ &>()));

public:
  constexpr static std::uint16_t numBits = _numBits;
  using Tail = decltype(std::declval<const BitsRng_ &>() |
                        drop(std::declval<std::size_t>() * numBits));

public:
  /**
   * @brief BitsWordFlow constructor from span.
   * @param data - bytes to create words.
   */
  explicit BitsWordFlow(std::span<const std::byte> data)
      : _data(data), _bitsRng(u64BitsRng_()), _intsRng(u64WordsRng_()) {}

  auto rng() const {
    return _intsRng;
  }

  /**
   * @brief getTail get tail bits.
   * @return tail bits array.
   */
  auto getTail() const { return _bitsRng | drop(rng().size() * numBits); }

private:
  auto u64BitsRng_() const { return ToBits<std::uint64_t>::perform(_data); }

  auto u64WordsRng_() const {
    return ToInts<std::uint64_t, numBits>::perform(_bitsRng);
  }

private:
  std::span<const std::byte> _data;
  BitsRng_ _bitsRng;
  IntsRng_ _intsRng;
};

#endif // BIT_WORD_FLOW_HPP
