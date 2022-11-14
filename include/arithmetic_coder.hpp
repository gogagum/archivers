#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>
#include <vector>

#include "arithmetic_coder_encoded.hpp"

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT>
class ArithmeticCoder {
public:

    using Word = typename FlowT::Word;

    using Res = ArithmeticCoderEncoded;

public:

    /**
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
    ArithmeticCoder(FlowT&& byteFlow);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    Res encode();

private:

    void _countProbabilities();

private:

    template <class Value>
    using MapWordTo = std::map<Word, Value, typename Word::Order>;

private:
    FlowT _wordFlow;
    MapWordTo<std::size_t> _cumulativeNumFound;
    std::size_t _nonEncodedSize;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT>
garchiever::ArithmeticCoder<FlowT>::ArithmeticCoder(FlowT&& symbolsFlow)
        : _wordFlow(symbolsFlow) {
    _countProbabilities();
}

//----------------------------------------------------------------------------//
template <class FlowT>
auto garchiever::ArithmeticCoder<FlowT>::encode() -> Res {

}

//----------------------------------------------------------------------------//
template <class FlowT>
void garchiever::ArithmeticCoder<FlowT>::_countProbabilities() {
    MapWordTo<std::size_t> numFound;

    for (auto word : _wordFlow) {
        numFound[word]++;
    }
    _nonEncodedSize = _wordFlow.bytesLeft();
    std::size_t currNum = 0;
    for (auto [word, num] : _cumulativeNumFound) {
        _cumulativeNumFound[word] = currNum;
        currNum += num;
    }
}

#endif // ARITHMETIC_CODER_HPP
