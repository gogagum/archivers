cmake_minimum_required(VERSION 3.5)

project(archievers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system program_options REQUIRED)

add_subdirectory(thirdparty/dynamic-segment-tree)
include_directories(${Boost_INCLUDE_DIRS}
    include/
    thirdparty/dynamic-segment-tree/include/)

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
    include/flow/bytes_word_flow.hpp
    include/flow/bits_word_flow.hpp
    include/word/bytes_word.hpp
    include/word/bits_word.hpp
    include/word/integer_word.hpp
    include/word/uint_word.hpp
)

target_link_libraries(encoding_lib dynamic-segment-tree)

add_library(common STATIC common.hpp common.cpp opt_ostream_ref.hpp opt_ostream_ref.cpp)

add_subdirectory(arithmetic_archiever)
add_subdirectory(arithmetic_a_archiever)
add_subdirectory(arithmetic_d_archiever)
add_subdirectory(ppmd_archiever)
add_subdirectory(numerical)

add_subdirectory(test)
