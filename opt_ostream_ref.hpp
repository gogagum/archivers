#ifndef OPT_OSTREAM_REF_HPP
#define OPT_OSTREAM_REF_HPP

#include <optional>
#include <ostream>

namespace optout {

using OptOstreamRef = std::optional<std::reference_wrapper<std::ostream>>;

template <class CharT>
OptOstreamRef operator<<(OptOstreamRef os, const CharT* str) {
    if (os.has_value()) {
        os.value().get() << str;
    }
    return os;
}

template <typename T>
OptOstreamRef operator<<(OptOstreamRef os, T printed) {
    if (os.has_value()) {
        os.value().get() << printed;
    }
    return os;
}

OptOstreamRef operator<<(OptOstreamRef os, std::ostream& (*func)(std::ostream&) );

}

using optout::operator<<;

#endif // OPT_OSTREAM_REF_HPP
