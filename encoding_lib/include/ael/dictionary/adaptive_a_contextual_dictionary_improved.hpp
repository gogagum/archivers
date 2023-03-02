#ifndef ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP
#define ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP

#include <unordered_map>
#include <cstdint>

#include "impl/contextual_dictionary_base_improved.hpp"
#include "adaptive_a_dictionary.hpp"

#include <stdexcept>

namespace ael::dict {

using AdaptiveAContextualDictionaryImproved =
    impl::ContextualDictionaryBaseImproved<AdaptiveADictionary>;

}

#endif  // ADAPTIVE_A_CONTEXTUAL_DICTIONARY_IMPROVED_HPP