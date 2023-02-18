#ifndef APPLIB_WORD_PACKER_HPP
#define APPLIB_WORD_PACKER_HPP

#include <ranges>
#include <ael/byte_data_constructor.hpp>
#include <applib/exceptions.hpp>
#include <applib/words_and_flow.hpp>

////////////////////////////////////////////////////////////////////////////////
/// \brief The WordPacker class
///
class WordPacker {
public:
    template <std::ranges::input_range RangeT>
    static void process(RangeT rng, ael::ByteDataConstructor& dataConstructor, std::uint16_t numBits) {

        #define BITS_DECODER_CASE(numBits) \
            case (numBits): _process<RangeT, (numBits)>(rng, dataConstructor); break;

        switch (numBits) {
            BITS_DECODER_CASE(8);
            BITS_DECODER_CASE(9);
            BITS_DECODER_CASE(10);
            BITS_DECODER_CASE(11);
            BITS_DECODER_CASE(12);
            BITS_DECODER_CASE(13);
            BITS_DECODER_CASE(14);
            BITS_DECODER_CASE(15);
            BITS_DECODER_CASE(16);
            BITS_DECODER_CASE(17);
            BITS_DECODER_CASE(18);
            BITS_DECODER_CASE(19);
            BITS_DECODER_CASE(20);
            BITS_DECODER_CASE(21);
            BITS_DECODER_CASE(22);
            BITS_DECODER_CASE(23);
            BITS_DECODER_CASE(24);
            BITS_DECODER_CASE(25);
            BITS_DECODER_CASE(26);
            BITS_DECODER_CASE(27);
            BITS_DECODER_CASE(28);
            BITS_DECODER_CASE(29);
            BITS_DECODER_CASE(30);
            BITS_DECODER_CASE(31);
            BITS_DECODER_CASE(32);
        default:
            throw UnsupportedDecodeBitsMode(numBits);
            break;
        }

        #undef BITS_DECODER_CASE
    }
private:
    template <std::ranges::input_range RangeT, std::uint16_t numBits>
    static void _process(RangeT rng, ael::ByteDataConstructor& dataConstructor) {
        for (std::uint64_t ord: rng) {
            _packWordIntoData(Word<numBits>::byOrd(ord), dataConstructor);
        }
    }

    //------------------------------------------------------------------------//
    template <std::uint8_t _numBytes>
    static void _packWordIntoData(const ael::w::BytesWord<_numBytes> word,
                                  auto& cntr) {
        word.bytesOut(cntr.getByteBackInserter());
    }

    //------------------------------------------------------------------------//
    template <std::uint16_t _numBits>
    static void _packWordIntoData(const ael::w::BitsWord<_numBits> word,
                                  auto& cntr) {
        word.bitsOut(cntr.getBitBackInserter());
    }
};

#endif