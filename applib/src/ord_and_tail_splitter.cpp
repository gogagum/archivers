#include <applib/ord_and_tail_splitter.hpp>
#include <applib/exceptions.hpp>

////////////////////////////////////////////////////////////////////////////////
auto OrdAndTailSplitter::process(
        const std::span<const std::byte>& inData,
        std::uint8_t numBits) -> Ret {
    #define FILE_SPLITTER_BITS_CASE(bits) \
        case (bits): return process_<bits>(inData);

    switch (numBits) {
        FILE_SPLITTER_BITS_CASE(8);
        FILE_SPLITTER_BITS_CASE(9);
        FILE_SPLITTER_BITS_CASE(10);
        FILE_SPLITTER_BITS_CASE(11);
        FILE_SPLITTER_BITS_CASE(12);
        FILE_SPLITTER_BITS_CASE(13);
        FILE_SPLITTER_BITS_CASE(14);
        FILE_SPLITTER_BITS_CASE(15);
        FILE_SPLITTER_BITS_CASE(16);
        FILE_SPLITTER_BITS_CASE(17);
        FILE_SPLITTER_BITS_CASE(18);
        FILE_SPLITTER_BITS_CASE(19);
        FILE_SPLITTER_BITS_CASE(20);
        FILE_SPLITTER_BITS_CASE(21);
        FILE_SPLITTER_BITS_CASE(22);
        FILE_SPLITTER_BITS_CASE(23);
        FILE_SPLITTER_BITS_CASE(24);
        FILE_SPLITTER_BITS_CASE(25);
        FILE_SPLITTER_BITS_CASE(26);
        FILE_SPLITTER_BITS_CASE(27);
        FILE_SPLITTER_BITS_CASE(28);
        FILE_SPLITTER_BITS_CASE(29);
        FILE_SPLITTER_BITS_CASE(30);
        FILE_SPLITTER_BITS_CASE(31);
        FILE_SPLITTER_BITS_CASE(32);
    default:
        throw UnsupportedEncodeBitsMode(numBits); break;
    }

    #undef FILE_SPLITTER_BITS_CASE
}
