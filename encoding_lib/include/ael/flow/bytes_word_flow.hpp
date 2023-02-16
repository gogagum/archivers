#ifndef BIT_FLOW_HPP
#define BIT_FLOW_HPP

#include <ael/word/bytes_word.hpp>

#include <span>
#include <cstddef>
#include <cstring>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/range/algorithm_ext.hpp>

namespace ga::fl {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ByteFlow class
///
template <std::uint8_t numBytes>
class BytesWordFlow {
private:
    using _Word = w::BytesWord<numBytes>;
public:
    using Tail = boost::container::static_vector<bool, 32>;
private:
    class _Iterator;
public:

    /**
     * @brief ByteFlow
     * @param bytes
     */
    BytesWordFlow(std::span<const std::byte> bytes) : _bytes(bytes) {}

    /**
     * @brief begin - beginning iterator getter.
     * @return beginning iterator.
     */
    [[nodiscard]] auto begin() const { return _Iterator(_bytes.data()); }

    /**
     * @brief end - ending iterator getter.
     * @return ending iterator.
     */
    [[nodiscard]] auto end() const { return _Iterator(_bytes.data()) + size(); }

    /**
     * @brief countNumberOfWords.
     * @return number of words.
     */
    [[nodiscard]] std::size_t size() const { return _bytes.size() / numBytes; }

    /**
     * @brief getTail
     * @return
     */
    [[nodiscard]] Tail getTail() const
    { return boost::accumulate(_getTailBytes(), Tail{}, tailAccOp); }

private:

    using _DataIterator = std::span<const std::byte>::iterator;

private:

    std::uint8_t _getTailBytesSize() const { return _bytes.size() % numBytes; }

    boost::iterator_range<_DataIterator> _getTailBytes() const
    { return {_bytes.end() - _getTailBytesSize(), _bytes.end()}; }

private:

    constexpr static Tail (*tailAccOp)(Tail&&, std::byte) =
        +[](Tail&& currTail, std::byte b) {
            auto bitRng = boost::make_iterator_range(impl::BitsIterator(b, 0),
                                                     impl::BitsIterator(b));
            return boost::range::push_back(currTail, bitRng);
        };

private:

    std::span<const std::byte> _bytes;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::uint8_t numBytes>
class BytesWordFlow<numBytes>::_Iterator : public boost::iterator_facade<
    _Iterator,
    _Word,
    boost::random_access_traversal_tag,
    _Word> {
public:
    using type = _Iterator;
private:
    //------------------------------------------------------------------------//
    explicit _Iterator(const std::byte* ptr) : _ptr(ptr) {};
protected:
    //------------------------------------------------------------------------//
    _Word dereference() const           { return _Word(_ptr); };
    //------------------------------------------------------------------------//
    bool equal(const type& other) const { return _ptr == other._ptr; };
    //------------------------------------------------------------------------//
    void increment()                    { _ptr += numBytes; };
    //------------------------------------------------------------------------//
    void advance(std::ptrdiff_t offset) { _ptr += numBytes * offset; }
private:
    const std::byte* _ptr;
private:
    friend class boost::iterators::iterator_core_access;
    friend class BytesWordFlow<numBytes>;
};

}  // namespace ga

#endif // BIT_FLOW_HPP
