#ifndef INT_RANGE_WORD_FLOW_HPP
#define INT_RANGE_WORD_FLOW_HPP

#include <vector>

#include "../word/int_range_word.hpp"

namespace ga::fl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerWordFlow class
///
template <class I, I low, I high>
class IntegerWordFlow {
public:
    using Sym = w::IntegerWord<I, low, high>;
public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The Iterator class
    ///
    class Iterator {
    private:
        /**
         * @brief Iterator
         * @param iter
         */
        Iterator(typename std::vector<I>::const_iterator iter) : _iter{ iter } {};
    public:
        Iterator& operator++() { ++_iter; return *this; };
        Iterator operator++(int) { auto ret = Iterator(_iter); ++_iter; return ret; }
        bool operator==(const Iterator& other) const = default;
        bool operator!=(const Iterator& other) const = default;
        w::IntegerWord<I,  low, high> operator*() const { return w::IntegerWord<I,  low, high>(*_iter); }
    private:
        typename std::vector<I>::const_iterator _iter;
    private:
        friend class IntegerWordFlow;
    };

public:

    IntegerWordFlow(std::vector<I>&& ints);

    /**
     * @brief begin - beginning iterator getter.
     * @return beginning iterator.
     */
    Iterator begin() const;

    /**
     * @brief end - ending iterator getter.
     * @return ending iterator.
     */
    Iterator end() const;

    /**
     * @brief countNumberOfWords.
     * @return number of words.
     */
    std::size_t getNumberOfWords() const;


private:
    std::vector<I> _ints;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, I high>
IntegerWordFlow<I, low, high>::IntegerWordFlow(std::vector<I>&& ints) : _ints(std::move(ints)) {}

//----------------------------------------------------------------------------//
template <class I, I low, I high>
auto IntegerWordFlow<I, low, high>::begin() const -> Iterator {
    return Iterator(_ints.begin());
}

//----------------------------------------------------------------------------//
template <class I, I low, I high>
auto IntegerWordFlow<I, low, high>::end() const -> Iterator {
    return Iterator(_ints.end());
}

//----------------------------------------------------------------------------//
template <class I, I low, I high>
std::size_t IntegerWordFlow<I, low, high>::getNumberOfWords() const {
    return _ints.size();
}


}  // namespace ga::fl

#endif // INT_RANGE_WORD_FLOW_HPP
