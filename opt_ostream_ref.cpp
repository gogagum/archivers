#include "opt_ostream_ref.hpp"

namespace optout {

OptOstreamRef operator<<(OptOstreamRef os, std::ostream& (*func)(std::ostream&)) {
    if (os.has_value()) {
        os = func(os.value().get());
    }
    return os;
}

}

