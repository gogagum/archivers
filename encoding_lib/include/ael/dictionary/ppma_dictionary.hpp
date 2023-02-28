#ifndef PPMA_DICTIONARY_HPP
#define PPMA_DICTIONARY_HPP

#include "ael/dictionary/adaptive_a_dictionary.hpp"
#include "ael/dictionary/impl/cumulative_count.hpp"
#include "word_probability_stats.hpp"

#include <boost/range/iterator_range.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

namespace ael::dict {

namespace bpm = boost::multiprecision;

class PPMADictionary : AdaptiveADictionary {
public:
    using Ord = std::uint64_t;
    using Count = bpm::uint256_t;
    using ProbabilityStats = WordProbabilityStats<Count>;
    constexpr const static std::uint16_t countNumBits = 240;
public:

    PPMADictionary(Ord maxOrd, std::size_t ctxLength)
        : _ctxLength(ctxLength),
          AdaptiveADictionary(maxOrd) {
        /**
         * \tau_{ctx}_{i} ~ M * 2^40
         * Producxt of tau-s must be less than (M * 2^40) ^ "tau-s count"
         * Estimation: (log_2(M) + 40) * l_{ctx} < maxCntBits.
         */
        const double maxOrdLog = std::log2(maxOrd);
        if ((maxOrdLog + 40) * ctxLength > countNumBits) {
            throw std::logic_error("Too big context.");
        }
    }

    Ord getWordOrd(Count cumulativeNumFound) const;

    ProbabilityStats getProbabilityStats(Ord ord) {
        assert(ord < this->_maxOrd);
        const auto ret = _getProbabilityStats(ord);
        _updateWordCnt(ord, 1);
        return ret;
    }

    Count getTotalWordsCnt() const {
        auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
        for (;
             !ctx.empty() && !_ctxInfo.contains(ctx);
             ctx.pop_back()) {
            // Skip contexts which were not found yet.
        }
        Count total = 0;
        for (;
             !ctx.empty();
             ctx.pop_back()) {
            const auto totalCnt = _ctxInfo.at(ctx).getTotalWordsCnt();  
            total = (total + 1) * totalCnt;
        }
        const auto totalCnt = AdaptiveADictionary::getTotalWordsCnt(); 
        total = (total + 1) * totalCnt;
        return total;
    }

private:
    using _SearchCtx = boost::container::static_vector<Ord, 16>;
    using _SearchCtxHash = boost::hash<_SearchCtx>;
private:

    Count _getLowerCumulativeCnt(Ord ord) const {
        if (ord >= this->_maxOrd) {
            return this->getTotalWordsCnt();
        }
        auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
        for (;
             !ctx.empty() && !_ctxInfo.contains(ctx);
             ctx.pop_back()) {
            // Skip contexts which were not found yet.
        }
        Count l = 0;
        for (;
             !ctx.empty();
             ctx.pop_back()) {
            l *= _ctxInfo.at(ctx).getTotalWordsCnt();
            l += _ctxInfo.at(ctx).getLowerCumulativeCount(ord);
        }
        l *= AdaptiveADictionary::getTotalWordsCnt();
        l += AdaptiveADictionary::_getLowerCumulativeCnt(ord);
        return l;
    }

    ProbabilityStats _getProbabilityStats(Ord ord) const {
        auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
        for (;
             !ctx.empty() && !_ctxInfo.contains(ctx);
             ctx.pop_back()) {
            // Skip contexts which were not found yet.
        }
        Count l = 0;
        Count r = 0;
        Count total = 0;
        for (;
             !ctx.empty();
             ctx.pop_back()) {
            const auto totalCnt = _ctxInfo.at(ctx).getTotalWordsCnt();  
            l *= totalCnt;
            r *= totalCnt;
            total = (total + 1) * totalCnt;
            const auto ctxL = _ctxInfo.at(ctx).getLowerCumulativeCount(ord);
            const auto ctxR = ctxL + _ctxInfo.at(ctx).getCount(ord);
            l += ctxL;
            r += ctxR;
        }
        const auto totalCnt = AdaptiveADictionary::getTotalWordsCnt(); 
        l *= totalCnt;
        r *= totalCnt;
        total = (total + 1) * totalCnt;
        const auto aDictL = AdaptiveADictionary::_getLowerCumulativeCnt(ord); 
        const auto aDictR = aDictL + AdaptiveADictionary::_getWordCnt(ord); 
        assert(aDictR > aDictL);
        l += aDictL;
        r += aDictR;
        assert(r > l);
        assert(total >= r);
        return {l, r, total};
    }

    void _updateWordCnt(Ord ord, AdaptiveADictionary::Count cnt) {
        for (auto ctx = _SearchCtx(_ctx.rbegin(), _ctx.rend());
             !ctx.empty();
             ctx.pop_back()) {
            if (!_ctxInfo.contains(ctx)) {
                _ctxInfo.emplace(ctx, impl::CumulativeCount(_maxOrd));
            }
            _ctxInfo.at(ctx).increaseOrdCount(ord, cnt);
        }
        AdaptiveADictionary::_updateWordCnt(ord, cnt);
        _ctx.push_back(ord);
        if (_ctx.size() > _ctxLength) {
            _ctx.pop_front();
        }
    }

private:
    std::deque<Ord> _ctx;
    std::unordered_map<_SearchCtx, impl::CumulativeCount, _SearchCtxHash> _ctxInfo;
    const std::size_t _ctxLength;
};

}

#endif