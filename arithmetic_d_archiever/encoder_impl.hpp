#ifndef ENCODE_IMPL_HPP
#define ENCODE_IMPL_HPP

#include "arithmetic_d_archiever_include.hpp"

#include "../common.hpp"
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
/// \brief The FileBitsDAdaptiveEncodeImpl class
///
template <std::uint16_t bitsNum>
struct DAdaptiveEncodeImpl {
    static void process(FileOpener& fileOpener,
                        auto outIter,
                        boost::container::static_vector<bool, 32>& tail) {
        auto flow = Flow<bitsNum>(fileOpener.getInData());
        std::transform(flow.begin(), flow.end(), outIter,
                       [](const Word<bitsNum>& w) {
                           return Word<bitsNum>::ord(w);
                       });
        auto flowTail = flow.getTail();
        tail = boost::container::static_vector<bool, 32>(flowTail.begin(), flowTail.end());
    }
};

#endif // ENCODE_IMPL_HPP
