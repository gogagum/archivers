#ifndef ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP
#define ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP

#include "impl/contextual_dictionary_base.hpp"
#include "adaptive_a_dictionary.hpp"

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
// \brief The adaptive contextual dictionary class
using AdaptiveAContextualDictionary = impl::ContextualDictionaryBase<AdaptiveADictionary>;

}

#endif  // ADAPTIVE_A_CONTEXTUAL_DICTIONARY_HPP