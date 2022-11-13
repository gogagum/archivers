#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include "byte_flow.hpp"
#include "bytes_symbol.hpp"

#include <map>

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <std::size_t wordLength>
class ArithmeticCoder {
public:
    class Res {};

public:

    /**
     * @brief ArithmeticCoder constructor from byte flow to encode.
     * @param byteFlow - byte flow.
     */
    ArithmeticCoder(ByteFlow&& byteFlow);

    /**
     * @brief encode - encode byte flow.
     * @param bitFlow - byte
     */
    Res encode() {

    }

private:

    void _countProbabilities();

private:
    ByteFlow _byteFlow;
    std::map<BytesSymbol<wordLength>, std::size_t> _probabilities;
    std::size_t _nonEncodedSize;
};

}  // namespace garchiever

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <std::size_t wordLength>
garchiever::ArithmeticCoder<wordLength>::ArithmeticCoder(ByteFlow&& byteFlow)
    : _byteFlow(byteFlow) { }

//----------------------------------------------------------------------------//
template <std::size_t wordLength>
void garchiever::ArithmeticCoder<wordLength>::_countProbabilities() {
    while (!_byteFlow.finished<wordLength>()) {
        _probabilities[_byteFlow.takeByteSymbol<wordLength>()]++;
    }
    _nonEncodedSize = _byteFlow.bytesLeft();
}

#endif // ARITHMETIC_CODER_HPP
