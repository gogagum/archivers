#ifndef ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include <unordered_map>
#include <cstdint>

#include "impl/contextual_dictionary_base_improved.hpp"
#include "adaptive_d_dictionary.hpp"

#include <stdexcept>

namespace ael::dict {

using AdaptiveDContextualDictionaryImproved =
    impl::ContextualDictionaryBaseImproved<AdaptiveDDictionary>;

}

#endif  // ADAPTIVE_D_CONTEXTUAL_DICTIONARY_IMPROVED_HPP