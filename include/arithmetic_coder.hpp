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

    std::int64_t _getCumulativeNumFound(const Word& word);

private:

    template <class Value>
    using MapWordTo = std::map<Word, Value, typename Word::Order>;

private:
    FlowT _wordFlow;
    MapWordTo<std::int64_t> _cumulativeNumFound;
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
    auto ret = Res();

    constexpr const auto wordsNum = static_cast<std::uint64_t>(Word::maxNum);

    std::uint64_t low = 0;
    std::uint64_t high = wordsNum;
    std::uint64_t btf = 0;

    for (auto sym : _wordFlow) {
        assert(high >= low && "high must be greater or equal than low");
        std::uint64_t range = high - low;


        auto nextSym = Word::nextSym(sym);

        high = low + range * _cumulativeNumFound[nextSym] / wordsNum;
        low = low + range * _cumulativeNumFound[sym] / wordsNum;

        while (true) {
            if (high < wordsNum / 2) {
                res.putBit(false);
                for (; btf != 0; --btf) {
                    res.putBit(true);
                }
                high = high * 2 + 1;
                low = low * 2;
            } else if (low >= wordsNum / 2) {
                res.putBit(true);
                for (; btf != 0; --btf) {
                    res.putBit(false);
                }
            } else if (low >= wordsNum / 4 && high < wordsNum * 3 / 4) {
                high = 2 * high - wordsNum / 2;
                low = 2 * low - wordsNum / 2;
                ++btf;
            } else {
                break;
            }
        }
    }


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

//----------------------------------------------------------------------------//
template <class FlowT>
void
garchiever::ArithmeticCoder<FlowT>::_getCumulativeNumFound(const Word &word) {
    lowerBoudIt = _cumulativeNumFound.lower_bound(word);
    if (lowerBoundIt == _cumulativeNumFound.begin()) {
        return 0;
    } else {
        return (lowerBoundIt - 1)->second;
    }
}

#endif // ARITHMETIC_CODER_HPP
