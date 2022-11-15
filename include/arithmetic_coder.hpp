#ifndef ARITHMETIC_CODER_HPP
#define ARITHMETIC_CODER_HPP

#include <map>
#include <vector>
#include <cassert>

#include "arithmetic_coder_encoded.hpp"

namespace garchiever {

////////////////////////////////////////////////////////////////////////////////
/// \brief The ArithmeticCoder class
///
template <class FlowT>
class ArithmeticCoder {
public:

    using Sym = typename FlowT::Sym;

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

    std::uint64_t _getCumulativeNumFoundLow(const Sym& word);

    std::uint64_t _getCumulativeNumFoundHigh(const Sym& word);

private:

    template <class Value>
    using MapSymTo = std::map<Sym, Value, typename Sym::Order>;

private:
    FlowT _wordFlow;
    MapSymTo<std::int64_t> _cumulativeNumFound;
    std::int64_t _totalSymsCount;
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

    ret.putT(static_cast<uint8_t>(Sym::numBytes));
    ret.putT(static_cast<uint64_t>(_cumulativeNumFound.size()));
    for (auto [word, cumulFound]: _cumulativeNumFound) {
        ret.putT(word);
        ret.putT(_cumulativeNumFound);
    }
    ret.putT(static_cast<uint64_t>(_totalSymsCount));

    constexpr auto wordsNum = static_cast<std::uint64_t>(Sym::maxNum);
    constexpr auto wordsNum_2 = wordsNum / 2;
    constexpr auto wordsNum_4 = wordsNum / 4;
    constexpr auto wordsNum_3to4 = 3 * wordsNum / 4;

    std::uint64_t low = 0;
    std::uint64_t high = wordsNum;
    std::size_t btf = 0;

    for (auto sym : _wordFlow) {
        assert(high >= low && "high must be greater or equal than low");
        std::uint64_t range = high - low;

        high = low + range * _getCumulativeNumFoundHigh(sym) / _totalSymsCount;
        low = low + range * _getCumulativeNumFoundLow(sym) / _totalSymsCount;

        while (true) {
            if (high <= wordsNum_2) {
                ret.putBit(false);
                ret.putBitsRepeat(true, btf);
                btf = 0;
                high = high * 2;
                low = low * 2;
            } else if (low >= wordsNum_2) {
                ret.putBit(true);
                ret.putBitsRepeat(false, btf);
                btf = 0;
                high = high * 2 - range;
                low = low * 2 - range;
            } else if (low >= wordsNum_4 && high <= wordsNum_3to4) {
                high = 2 * high - wordsNum_2;
                low = 2 * low - wordsNum_2;
                ++btf;
            } else {
                break;
            }
        }
    }

    if (low <= wordsNum_4) {
        ret.putBitsRepeat(false, btf);
    } else {
        ret.putBitsRepeat(false, btf);
    }
    return ret;
}

//----------------------------------------------------------------------------//
template <class FlowT>
void garchiever::ArithmeticCoder<FlowT>::_countProbabilities() {
    MapSymTo<std::uint64_t> numFound;
    for (auto word : _wordFlow) {
        numFound[word]++;
    }
    _nonEncodedSize = _wordFlow.bytesLeft();
    _totalSymsCount = 0;
    for (auto [word, num] : _cumulativeNumFound) {
        _cumulativeNumFound[word] = _totalSymsCount;
        _totalSymsCount += num;
    }
}

//----------------------------------------------------------------------------//
template <class FlowT>
std::uint64_t
garchiever::ArithmeticCoder<FlowT>::_getCumulativeNumFoundLow(const Sym &word) {
    assert(_cumulativeNumFound.contains(word));
    return _cumulativeNumFound[word];
}


//----------------------------------------------------------------------------//
template <class FlowT>
std::uint64_t
garchiever::ArithmeticCoder<FlowT>::_getCumulativeNumFoundHigh(const Sym &word) {
    assert(_cumulativeNumFound.contains(word));
    auto iter = _cumulativeNumFound.find(word);
    if (auto nextIter = std::next(iter); nextIter == _cumulativeNumFound.end()) {
        return _totalSymsCount;
    } else {
        return nextIter->second;
    }
}

#endif // ARITHMETIC_CODER_HPP
