#include "uniform_dictionary.hpp"

namespace ga::dict {

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
auto UniformDictionary::getWordOrd(Count cumulativeNumFound) const -> Ord {
    return cumulativeNumFound;
}

//----------------------------------------------------------------------------//
auto UniformDictionary::getProbabilityStats(Ord ord) -> ProbabilityStats {
    return { ord, ord + 1, _maxOrd };
}

}  // namespace ga::dict