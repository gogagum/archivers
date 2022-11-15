#ifndef I_WORDS_FLOW_HPP
#define I_WORDS_FLOW_HPP

#include <boost/container/static_vector.hpp>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IWordsFlow class
///
template <class ThisT, class SymT>
class IWordsFlow {
public:
    using This = ThisT;
    using Sym = SymT;
};

}

#endif // I_WORDS_FLOW_HPP
