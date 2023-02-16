cmake_minimum_required(VERSION 3.5)

project(archievers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system program_options REQUIRED)

add_subdirectory(thirdparty/dynamic-segment-tree)
add_subdirectory(encoding_lib)

include_directories(${Boost_INCLUDE_DIRS}
                    encoding_lib/include/
                    thirdparty/dynamic-segment-tree/include/)

add_library(common STATIC common.hpp common.cpp opt_ostream_ref.hpp opt_ostream_ref.cpp)

add_subdirectory(arithmetic_archiever)
add_subdirectory(arithmetic_a_archiever)
add_subdirectory(arithmetic_d_archiever)
add_subdirectory(ppmd_archiever)
add_subdirectory(numerical)

add_subdirectory(test)
