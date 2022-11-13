#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT>
class ArithmeticCoder {
public:

    using Word = typename FlowT::Word;

    class Res {};

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
    Res encode() {

    }

private:

    void _countProbabilities();

    void _countCumulativeProbabilities();

    void _countLengthes();

private:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The _ByteSymbolInfo class
    struct _WordInfo{
        std::size_t numFound;
        std::size_t cumulativeNumFound;
        std::size_t length;
    };

private:
    FlowT _wordFlow;
    std::map<Word, _WordInfo, typename Word::Order> _wordInfos;
    std::size_t _nonEncodedSize;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class FlowT>
garchiever::ArithmeticCoder<FlowT>::ArithmeticCoder(FlowT&& symbolsFlow)
    : _wordFlow(symbolsFlow) { }

//----------------------------------------------------------------------------//
template <class FlowT>
void garchiever::ArithmeticCoder<FlowT>::_countProbabilities() {
    while (!_wordFlow.finished()) {
        _wordInfos[_wordFlow.takeWord()].numFound++;
    }
    _nonEncodedSize = _wordFlow.bytesLeft();
}

//----------------------------------------------------------------------------//
template <class FlowT>
void garchiever::ArithmeticCoder<FlowT>::_countCumulativeProbabilities() {
    std::size_t currNum = 0;
    for (auto entry : _wordInfos) {
        auto& info = entry.second;
        info.cumulativeNumFound = currNum;
        currNum += info.numFound;
    }
}

//----------------------------------------------------------------------------//
template <class FlowT>
void garchiever::ArithmeticCoder<FlowT>::_countLengthes() {
    for (auto entry : _wordInfos) {
        auto& info = entry.second;
    }
}

#endif // ARITHMETIC_CODER_HPP
