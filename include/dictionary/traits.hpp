#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <utility>

namespace ga::dict::traits {

template <class DictT>
concept needWordIncrease = requires(DictT a) {
    { a.increaseWordCount(std::declval<typename DictT::Word>()) };
};

}

#endif // TRAITS_HPP
