#include <iostream>

#include <applib/opt_ostream.hpp>
#include <applib/exceptions.hpp>

namespace optout {

////////////////////////////////////////////////////////////////////////////////
OptOstreamRef operator<<(OptOstreamRef os,
                         std::ostream& (*func)(std::ostream&)) {
    if (os.has_value()) {
        os = func(os.value().get());
    }
    return os;
}

}

//----------------------------------------------------------------------------//
optout::OptOstreamRef get_out_stream(const std::string& arg) {
    optout::OptOstreamRef outStream;

    if (arg == "stdout") {
        outStream = std::cout;
    } else if (arg == "stderr") {
        outStream = std::cerr;
    } else if (arg == "off") {
    } else {
        throw InvalidStreamParam(arg);
    }

    return outStream;
}