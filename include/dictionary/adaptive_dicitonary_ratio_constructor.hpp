#ifndef ADAPTIVE_DICITONARY_RATIO_CONSTRUCTOR_HPP
#define ADAPTIVE_DICITONARY_RATIO_CONSTRUCTOR_HPP

#include "adaptive_dictionary.hpp"

#include <cstdint>

namespace ga::dict::construct {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryRatioConstructor class
///
template<class DictT>
class AdaptiveDictionaryRatioConstructor;

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryRatioConstructor class
///
template <class WordT, class CountT>
class AdaptiveDictionaryRatioConstructor<AdaptiveDictionary<WordT, CountT>> {
public:
    using Dict = AdaptiveDictionary<WordT, CountT>;
public:
    //------------------------------------------------------------------------//
    AdaptiveDictionaryRatioConstructor(std::uint64_t ratio) : _ratio(ratio) {}
    //------------------------------------------------------------------------//
    Dict construct() const { return AdaptiveDictionary<WordT, CountT>(_ratio); }
private:
    std::uint64_t _ratio;
};

}

#endif // ADAPTIVE_DICITONARY_RATIO_CONSTRUCTOR_HPP
