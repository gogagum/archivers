#ifndef APPLIB_WORD_PACKER_HPP
#define APPLIB_WORD_PACKER_HPP

#include <ael/byte_data_constructor.hpp>
#include <applib/exceptions.hpp>
#include <applib/u64_tail_bits.hpp>
#include <applib/u64_tail_bytes.hpp>
#include <applib/words_and_flow.hpp>

////////////////////////////////////////////////////////////////////////////////
/// \brief The WordPacker class
///
class WordPacker {
 private:
  struct ToBits {
    std::size_t numBits;
    auto operator()(std::uint64_t ord) const {
      return u64_tail_bits(ord, numBits);
    }
  };

  struct ToBytes {
    std::size_t numBits;
    auto operator()(std::uint64_t ord) const {
      return u64_tail_bytes(ord, numBits / CHAR_BIT);
    }
  };

 public:
  template <std::ranges::input_range RangeT>
  static void process(RangeT&& rng, ael::ByteDataConstructor& dataConstructor,
                      std::uint16_t numBits) {
    using std::ranges::copy;
    using std::views::join;
    using std::views::transform;

    if (numBits % CHAR_BIT == 0) {
      copy(std::forward<RangeT>(rng) | transform(ToBytes{numBits}) | join,
           dataConstructor.getByteBackInserter());
    } else {
      copy(std::forward<RangeT>(rng) | transform(ToBits{numBits}) | join,
           dataConstructor.getBitBackInserter());
    }
  }
};

#endif  // APPLIB_WORD_PACKER_HPP
