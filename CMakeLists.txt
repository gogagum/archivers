cmake_minimum_required(VERSION 3.5)

project(archiever_encoder LANGUAGES CXX)
project(archiever_decoder LANGUAGES CXX)
project(archiever_tests LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(archiever_encoder encoder.cpp
        include/arithmetic_coder.hpp
        include/byte_data_constructor.hpp
        include/byte_data_constructor.cpp
        include/dictionary/uniform_dictionary.hpp
        include/dictionary/static_dictionary.hpp
        include/dictionary/adaptive_dictionary.hpp
        include/dictionary/integer_random_access_iterator.hpp
        include/flow/byte_flow.hpp
        include/word/bytes_symbol.hpp
    )
    add_executable(archiever_decoder decoder.cpp
        include/arithmetic_decoder.hpp
        include/data_parser.hpp
        include/data_parser.cpp
        include/dictionary/uniform_dictionary.hpp
        include/dictionary/static_dictionary.hpp
        include/dictionary/adaptive_dictionary.hpp
        include/dictionary/integer_random_access_iterator.hpp
        include/word/bytes_symbol.hpp
    )
    install(TARGETS archiever_encoder archiever_decoder
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(archiever_tests
    test/bits_iterator.cpp
    test/byte_data_constructor.cpp
    test/bytes_symbol.cpp
    test/data_parser.cpp
    test/dynamic_dictionary.cpp
    test/integer_word.cpp
    test/integer_word_flow.cpp
    test/static_dictionary.cpp
    test/uniform_dictionary.cpp
    include/word/bytes_symbol.hpp
    include/word/int_range_word.hpp
    include/byte_data_constructor.cpp
    include/data_parser.cpp
    include/flow/int_range_word_flow.hpp
    include/misc.hpp
)

target_link_libraries(archiever_tests LINK_PUBLIC gtest_main)
include(GoogleTest)
gtest_discover_tests(archiever_tests)
