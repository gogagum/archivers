project("arithmetic-encoding-lib")

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/adaptive_a_dictionary.cpp
        src/adaptive_d_dictionary.cpp
        src/adaptive_dictionary.cpp
        src/byte_data_constructor.cpp
        src/data_parser.cpp
        src/decreasing_on_update_dictionary.cpp        
        src/ppmd_dictionary.cpp
        src/static_dictionary.cpp
        src/uniform_dictionary.cpp
        
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


# without it public headers won't get installed
set(public_headers
    include/ael/arithmetic_coder.hpp
    include/ael/arithmetic_decoder.hpp
    include/ael/bits_iterator.hpp
    include/ael/byte_data_constructor.hpp
    include/ael/data_parser.hpp
    include/ael/ranges_calc.hpp
    include/ael/dictionary/impl/a_d_dictionary_base.hpp
    include/ael/dictionary/uniform_dictionary.hpp
    include/ael/dictionary/static_dictionary.hpp
    include/ael/dictionary/adaptive_dictionary.hpp
    include/ael/dictionary/adaptive_a_dictionary.hpp
    include/ael/dictionary/adaptive_d_dictionary.hpp
    include/ael/dictionary/ppmd_dictionary.hpp
    include/ael/dictionary/decreasing_counts_dictionary.hpp
    include/ael/dictionary/decreasing_on_update_dictionary.hpp
    include/ael/dictionary/integer_random_access_iterator.hpp
    include/ael/dictionary/word_probability_stats.hpp
    include/ael/flow/bytes_word_flow.hpp
    include/ael/flow/bits_word_flow.hpp
    include/ael/word/bytes_word.hpp
    include/ael/word/bits_word.hpp
    include/ael/word/integer_word.hpp
    include/ael/word/uint_word.hpp
)

# note that ${public_headers} has to be in quotes
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(arithmetic-encoding-lib dynamic-segment-tree)
