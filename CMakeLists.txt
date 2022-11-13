cmake_minimum_required(VERSION 3.5)

project(archiever LANGUAGES CXX)

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
        include/byte_flow.hpp
        include/bytes_symbol.hpp)
    target_link_libraries(archiever ${Boost_LIBRARIES})
    install(TARGETS archiever
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
