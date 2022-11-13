#ifndef I_WORDS_FLOW_HPP
#define I_WORDS_FLOW_HPP

#include <boost/container/static_vector.hpp>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The IWordsFlow class
///
template <class ThisT, class WordT>
class IWordsFlow {
public:
    using This = ThisT;
    using Word = WordT;
    using Tail = boost::container::static_vector<std::byte, Word::bytesSize>;
public:
    /**
     * @brief getTail
     * @return
     */
    virtual Tail getTail() = 0;

    virtual std::size_t getTailSize() const = 0;
};

}

#endif // I_WORDS_FLOW_HPP
