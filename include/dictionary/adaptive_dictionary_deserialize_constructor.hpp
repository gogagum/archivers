#ifndef ADAPTIVE_DICTIONARY_DESERIALIZE_CONSTRUCTOR_HPP
#define ADAPTIVE_DICTIONARY_DESERIALIZE_CONSTRUCTOR_HPP

#include "adaptive_dictionary.hpp"
#include "../data_parser.hpp"

#include <cstdint>

namespace ga::dict::construct {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryDeserializeConstructor class
///
template <class DictT>
class AdaptiveDictionaryDeserializeConstructor;

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryDeserializeConstructor class
///
template <class WordT, class CountT>
class AdaptiveDictionaryDeserializeConstructor<AdaptiveDictionary<WordT, CountT>> {
public:
    using Dict = AdaptiveDictionary<WordT, CountT>;

public:
    //------------------------------------------------------------------------//
    AdaptiveDictionaryDeserializeConstructor(ga::DataParser& data)
        : _ratio(data.takeT<std::uint64_t>()) {};
    //------------------------------------------------------------------------//
    Dict construct() {
        return AdaptiveDictionary<WordT, CountT>(_ratio);
    }
private:
    std::uint64_t _ratio;
};
}

#endif // ADAPTIVE_DICTIONARY_DESERIALIZE_CONSTRUCTOR_HPP
