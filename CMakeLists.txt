cmake_minimum_required(VERSION 3.5)

project(archievers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(archievers main.cpp
    include/arithmetic_coder.hpp
    include/byte_flow.hpp
    include/bytes_symbol.hpp)

install(TARGETS archievers
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
