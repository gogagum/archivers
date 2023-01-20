#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <utility>

namespace ga::dict::traits {

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <class DictT, class DestT>
constexpr const bool needSerialize = requires(const DictT& a, DestT& dataConstructor) {
    { a.serialize(dataConstructor) };
};

}

#endif // TRAITS_HPP
