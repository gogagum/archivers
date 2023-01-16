#ifndef ADAPTIVE_DICTIONARY_DESERIALIZE_CONSTRUCTOR_HPP
#define ADAPTIVE_DICTIONARY_DESERIALIZE_CONSTRUCTOR_HPP

#include "adaptive_dictionary.hpp"

#include <cstdint>

namespace ga::dict::construct {

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryDeserializeConstructor class
///
template <class DictT, class DataParser>
class AdaptiveDictionaryDeserializeConstructor;

////////////////////////////////////////////////////////////////////////////////
/// \brief The AdaptiveDictionaryDeserializeConstructor class
///
template <class WordT, class CountT, class DataParser>
class AdaptiveDictionaryDeserializeConstructor<AdaptiveDictionary<WordT, CountT>, DataParser> {
public:
    using Dict = AdaptiveDictionary<WordT, CountT>;

public:
    //------------------------------------------------------------------------//
    AdaptiveDictionaryDeserializeConstructor(DataParser& data)
        : _ratio(data.template takeT<std::uint64_t>()) {};
    //------------------------------------------------------------------------//
    Dict operator()() { return AdaptiveDictionary<WordT, CountT>(_ratio); }
private:
    std::uint64_t _ratio;
};
}

#endif // ADAPTIVE_DICTIONARY_DESERIALIZE_CONSTRUCTOR_HPP
