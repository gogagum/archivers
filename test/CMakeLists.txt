include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(archiever_tests
    bits_iterator.cpp
    bytes_iterator.cpp
    bits_word.cpp
    bits_word_flow.cpp
    byte_data_constructor.cpp
    bytes_word.cpp
    bytes_word_flow.cpp
    data_parser.cpp
    adaptive_dictionary.cpp
    static_dictionary.cpp
    uniform_dictionary.cpp
    adaptive_dictionary.cpp
    adaptive_a_dictionary.cpp
    adaptive_d_dictionary.cpp
    ppmd_dictionary.cpp
    ppma_dictionary.cpp
    decreasing_counts_dictionary.cpp
    decreasing_on_update_dictionary.cpp
)

target_link_libraries(archiever_tests LINK_PUBLIC gtest_main arithmetic-encoding-lib)

include(GoogleTest)
gtest_discover_tests(archiever_tests)
