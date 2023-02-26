#ifndef WORD_PROBABILITY_STATS_HPP
#define WORD_PROBABILITY_STATS_HPP

#include <cstdint>

namespace ael::dict {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ProbabilityStats class
///
struct WordProbabilityStats {
    std::uint64_t low;
    std::uint64_t high;
    std::uint64_t total;
};

}

#endif // WORD_PROBABILITY_STATS_HPP
