#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <utility>
#include "../byte_data_constructor.hpp"

namespace ga::dict::traits {

////////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <class DictT>
constexpr const bool needWordIncrease = requires(DictT a) {
    { a.increaseWordCount(std::declval<typename DictT::Word>()) };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <class DictT, class DestT>
constexpr const bool needSerialize = requires(const DictT& a, DestT& dataConstructor) {
    { a.serialize(dataConstructor) };
};

//----------------------------------------------------------------------------//
template <class DictT, class ConstructionTag>
constexpr const bool constructionTypeIs =
        std::is_same_v<typename DictT::ConstructionTag, ConstructionTag>;

}

#endif // TRAITS_HPP
