#ifndef BYTES_WORD_FLOW_HPP
#define BYTES_WORD_FLOW_HPP

#include "applib/flow/impl/to_bits.hpp"
#include "applib/flow/impl/to_ints.hpp"
#include <boost/container/static_vector.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ranges>
#include <span>

using std::views::drop;
using std::views::transform;

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes> class BytesWordFlow {
public:
  /**
   * @brief ByteFlow
   * @param bytes
   */
  BytesWordFlow(std::span<const std::byte> bytes) : _bytes(bytes) {}

  auto rng() const { return u64WordsRng(); }

  /**
   * @brief getTail
   * @return
   */
  auto getTail() const {
    return ToBits<std::byte>::perform(u8BytesRng() | drop(rng().size() * numBytes)) |
           transform([](std::byte b) { return b == std::byte{1}; });
  }

private:
  auto u8BytesRng() const {
    return _bytes | transform(std::to_integer<std::uint8_t>);
  }

  auto u64WordsRng() const {
    return ToInts<std::uint64_t, numBytes>::perform(
        u8BytesRng(), std::integral_constant<std::size_t, 8>{});
  }

private:
  std::span<const std::byte> _bytes;
};

#endif // BIT_FLOW_HPP
