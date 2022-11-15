cmake_minimum_required(VERSION 3.5)

project(archiever LANGUAGES CXX)
project(archiever_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(archiever main.cpp
        include/arithmetic_coder.hpp
        include/arithmetic_coder_encoded.hpp
        include/arithmetic_decoder.hpp
        include/arithmetic_decoder_decoded.hpp
        include/byte_flow.hpp
        include/bytes_symbol.hpp
        )
    target_link_libraries(archiever ${Boost_LIBRARIES})
    install(TARGETS archiever
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
    archiever_tests
    test.cpp
)

target_link_libraries(archiever_tests LINK_PUBLIC gtest_main)
include(GoogleTest)
gtest_discover_tests(archiever_tests)
