#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <type_traits>

namespace ga::fl::traits {

template <class FlowT>
using WordT = std::decay_t<decltype(*std::declval<FlowT>().begin())>;

}

#endif // TRAITS_HPP
