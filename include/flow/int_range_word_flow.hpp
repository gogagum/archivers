#ifndef INT_RANGE_WORD_FLOW_HPP
#define INT_RANGE_WORD_FLOW_HPP

#include <vector>
#include <boost/container/static_vector.hpp>

#include "word/integer_word.hpp"

namespace ga::fl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IntegerWordFlow class
///
template <class I, I low, std::uint16_t numBits>
class IntegerWordFlow {
public:
    using Tail = boost::container::static_vector<bool, numBits>;
    using Word = w::IntegerWord<I, low, numBits>;
    using Ord = typename Word::Ord;
    static constexpr std::uint8_t numBytes = (numBits - 1) / 8 + 1;
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
        Iterator(typename std::vector<I>::const_iterator iter);
    public:
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const = default;
        bool operator!=(const Iterator& other) const = default;
        w::IntegerWord<I,  low, numBits> operator*() const;
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

    /**
     * @brief getTail
     * @return
     */
    Tail getTail() const;

private:
    std::vector<I> _ints;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
IntegerWordFlow<I, low, numBits>::IntegerWordFlow(std::vector<I>&& ints)
    : _ints(std::move(ints)) {}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
auto IntegerWordFlow<I, low, numBits>::begin() const -> Iterator {
    return Iterator(_ints.begin());
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
auto IntegerWordFlow<I, low, numBits>::end() const -> Iterator {
    return Iterator(_ints.end());
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
std::size_t IntegerWordFlow<I, low, numBits>::getNumberOfWords() const {
    return _ints.size();
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
auto IntegerWordFlow<I, low, numBits>::getTail() const -> Tail {
    return {};
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
IntegerWordFlow<I, low, numBits>::Iterator::Iterator(
        typename std::vector<I>::const_iterator iter) : _iter{ iter } {};

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
auto IntegerWordFlow<I, low, numBits>::Iterator::operator++() -> Iterator& {
    ++_iter;
    return *this;
};

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
auto IntegerWordFlow<I, low, numBits>::Iterator::operator++(int) -> Iterator {
    auto ret = Iterator(_iter);
    ++_iter;
    return ret;
}

//----------------------------------------------------------------------------//
template <class I, I low, std::uint16_t numBits>
w::IntegerWord<I,  low, numBits>
IntegerWordFlow<I, low, numBits>::Iterator::operator*() const {
    return w::IntegerWord<I,  low, numBits>(*_iter);
}

}  // namespace ga::fl

#endif // INT_RANGE_WORD_FLOW_HPP
