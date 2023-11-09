#ifndef APPLIB_ORD_AND_TAIL_SPLITTER
#define APPLIB_ORD_AND_TAIL_SPLITTER

#include <algorithm>
#include <applib/words_and_flow.hpp>
#include <boost/container/static_vector.hpp>
#include <cstdint>
#include <span>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBytesAAdaptiveEncodeImpl class
///
class OrdAndTailSplitter {
  constexpr static auto maxTailSize = 32uz;

 public:
  using Tail = boost::container::static_vector<bool, maxTailSize>;

  struct Ret {
    std::vector<std::uint64_t> ords;
    Tail tail;
  };

 public:
  static Ret process(const std::span<const std::byte>& inData,
                     std::uint8_t numBits);

 private:
  template <std::uint8_t BitsNum>
  static Ret process_(const std::span<const std::byte>& inData);
};

////////////////////////////////////////////////////////////////////////////////
template <std::uint8_t BitsNum>
auto OrdAndTailSplitter::process_(const std::span<const std::byte>& inData)
    -> Ret {
  auto flow = typename TypeChoice<BitsNum>::Flow(inData);
  auto flowTail = flow.getTail();
  auto retTail = Tail(flowTail.begin(), flowTail.end());
  auto ords = std::vector<std::uint64_t>(flow.rng().size());
  std::ranges::copy(flow.rng(), ords.begin());
  return {ords, retTail};
}

#endif  // APPLIB_ORD_AND_TAIL_SPLITTER
