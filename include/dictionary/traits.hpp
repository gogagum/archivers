#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <utility>

namespace ga::dict::traits {


namespace impl {
////////////////////////////////////////////////////////////////////////////////
/// \brief The NeedWordIncrease class
///
template <class DictT>
struct NeedWordIncrease {
    constexpr static bool ret = requires(DictT a) {
        { a.increaseWordCount(std::declval<typename DictT::Word>()) };
    };
};
}

//----------------------------------------------------------------------------//
template <class DictT>
constexpr const bool needWordIncrease = impl::NeedWordIncrease<DictT>::ret;

//----------------------------------------------------------------------------//
template <class DictT, class ConstructionTag>
constexpr const bool constructionTypeIs =
        std::is_same_v<typename DictT::ConstructionTag, ConstructionTag>;

}

#endif // TRAITS_HPP
