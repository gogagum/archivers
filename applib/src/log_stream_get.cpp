#include <applib/log_stream_get.hpp>
#include <ostream>
#include <iostream>

#include <fmt/format.h>

////////////////////////////////////////////////////////////////////////////////
std::ostream& LogStreamGet::getLogStream(const std::string &strParam) {
    if (strParam == "stdout") {
        return std::cout;
    } else if (strParam == "stderr") {
        return std::cerr;
    } else {
        if (strParam != "off") {
            throw std::logic_error(fmt::format(
                    "{} is invalig log-stream parameter",
                    strParam)
                );
        }
        return _nullStream;
    }
}

////////////////////////////////////////////////////////////////////////////////
std::ostream LogStreamGet::_nullStream{nullptr};
