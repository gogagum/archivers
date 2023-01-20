cmake_minimum_required(VERSION 3.5)

project(archiever_encoder LANGUAGES CXX)
project(archiever_decoder LANGUAGES CXX)
project(archiever_tests LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system program_options REQUIRED)

add_library(encoding_lib STATIC
    include/arithmetic_coder.hpp
    include/arithmetic_decoder.hpp
    include/bits_iterator.hpp
    include/byte_data_constructor.hpp
    include/byte_data_constructor.cpp
    include/data_parser.hpp
    include/data_parser.cpp
    include/ranges_calc.hpp
    include/dictionary/uniform_dictionary.hpp
    include/dictionary/static_dictionary.hpp
    include/dictionary/adaptive_dictionary.hpp
    include/dictionary/adaptive_a_dictionary.hpp
    include/dictionary/adaptive_d_dictionary.hpp
    include/dictionary/decreasing_counts_dictionary.hpp
    include/dictionary/integer_random_access_iterator.hpp
    include/dictionary/traits.hpp
    include/flow/int_range_word_flow.hpp
    include/flow/bytes_word_flow.hpp
    include/flow/bits_word_flow.hpp
    include/word/bytes_word.hpp
    include/word/bits_word.hpp
    include/word/integer_word.hpp
    include/word/word_ord_comp.hpp
)

include_directories(${Boost_INCLUDE_DIRS} include/)
add_executable(archiever_encoder encoder.cpp file_opener.cpp)
target_link_libraries(archiever_encoder encoding_lib)

add_executable(archiever_decoder decoder.cpp file_opener.cpp)
target_link_libraries(archiever_decoder encoding_lib)

target_link_libraries(archiever_encoder ${Boost_LIBRARIES})
target_link_libraries(archiever_decoder ${Boost_LIBRARIES})

install(TARGETS archiever_encoder archiever_decoder
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(archiever_tests
    test/bits_iterator.cpp
    test/bits_word.cpp
    test/bits_word_flow.cpp
    test/byte_data_constructor.cpp
    test/bytes_word.cpp
    test/bytes_word_flow.cpp
    test/data_parser.cpp
    test/adaptive_dictionary.cpp
    test/integer_word.cpp
    test/integer_word_flow.cpp
    test/static_dictionary.cpp
    test/uniform_dictionary.cpp
    test/adaptive_dictionary.cpp
    test/adaptive_a_dictionary.cpp
    test/adaptive_d_dictionary.cpp
    test/arithmetic_coder.cpp
)

target_link_libraries(archiever_tests LINK_PUBLIC gtest_main encoding_lib)

include(GoogleTest)
gtest_discover_tests(archiever_tests)
