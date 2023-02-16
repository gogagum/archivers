project(encoding_lib)

add_library(encoding_lib STATIC
    include/arithmetic_coder.hpp
    include/arithmetic_decoder.hpp
    include/bits_iterator.hpp
    include/byte_data_constructor.hpp
    include/byte_data_constructor.cpp
    include/data_parser.hpp
    include/data_parser.cpp
    include/ranges_calc.hpp
    include/dictionary/impl/a_d_dictionary_base.hpp
    include/dictionary/uniform_dictionary.hpp
    include/dictionary/uniform_dictionary.cpp
    include/dictionary/static_dictionary.hpp
    include/dictionary/static_dictionary.cpp
    include/dictionary/adaptive_dictionary.hpp
    include/dictionary/adaptive_dictionary.cpp
    include/dictionary/adaptive_a_dictionary.hpp
    include/dictionary/adaptive_a_dictionary.cpp
    include/dictionary/adaptive_d_dictionary.hpp
    include/dictionary/adaptive_d_dictionary.cpp
    include/dictionary/ppmd_dictionary.hpp
    include/dictionary/ppmd_dictionary.cpp
    include/dictionary/decreasing_counts_dictionary.hpp
    include/dictionary/decreasing_on_update_dictionary.hpp
    include/dictionary/decreasing_on_update_dictionary.cpp
    include/dictionary/integer_random_access_iterator.hpp
    include/dictionary/word_probability_stats.hpp
    include/flow/bytes_word_flow.hpp
    include/flow/bits_word_flow.hpp
    include/word/bytes_word.hpp
    include/word/bits_word.hpp
    include/word/integer_word.hpp
    include/word/uint_word.hpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(encoding_lib dynamic-segment-tree)