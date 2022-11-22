#pragma once

#ifndef I_WORDS_FLOW_HPP
#define I_WORDS_FLOW_HPP

#include <cstdint>
#include <boost/container/static_vector.hpp>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IWordsFlow class
///
template <class ThisT, class SymT>
class ISymFlow {
public:
    using This = ThisT;
    using Sym = SymT;
public:

    virtual std::uint8_t getTailSize() const = 0;

    virtual boost::container::static_vector<std::byte, Sym::numBytes> getTail() const = 0;
};

}

#endif // I_WORDS_FLOW_HPP
