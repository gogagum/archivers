cmake_minimum_required(VERSION 3.15)

project(archievers LANGUAGES CXX)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if (CMAKE_CROSSCOMPILING)
    message("Cross compiling.")
    message("Compile target: ${CMAKE_CXX_COMPILER_TARGET}")
    if (${CMAKE_CXX_COMPILER_TARGET} STREQUAL "x86_64-w64-mingw32")
        set(BOOST_ROOT "/usr/x86_64-w64-mingw32/lib/cmake/Boost-1.81.0")
        set(BOOST_INCLUDEDIR "/usr/x86_64-w64-mingw32/include")
        set(BOOST_LIBRARYDIR "/usr/x86_64-w64-mingw32/lib")
    elseif(${CMAKE_CXX_COMPILER_TARGET} STREQUAL "i686-w64-mingw32")
        set(BOOST_ROOT "/usr/i686-w64-mingw32/lib/cmake/Boost-1.81.0")
        set(BOOST_INCLUDEDIR "/usr/i686-w64-mingw32/include")
        set(BOOST_LIBRARYDIR "/usr/i686-w64-mingw32/lib")
    else()
        message( FATAL_ERROR "Can not cross compile for target ${CMAKE_CXX_COMPILER_TARGET}." )
    endif()
else()
    message("Not cross compiling.")
    message("Compile target: ${CMAKE_CXX_COMPILER_TARGET}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

message("BOOST_ROOT:${BOOST_ROOT}")
find_package(Boost 1.81.0 REQUIRED COMPONENTS program_options)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

include_directories(thirdparty/dynamic-segment-tree/include/)

include_directories(thirdparty/dynamic-segment-tree/include/)
include_directories(encoding_lib/include/)
add_subdirectory(applib)
include_directories(applib/include)
add_subdirectory(arithmetic_archiever)
add_subdirectory(arithmetic_a_archiever)
add_subdirectory(arithmetic_a_contextual_archiever)
add_subdirectory(arithmetic_a_contextual_archiever_improved)
add_subdirectory(arithmetic_d_archiever)
add_subdirectory(arithmetic_d_contextual_archiever)
add_subdirectory(arithmetic_d_contextual_archiever_improved)
add_subdirectory(ppma_archiever)
add_subdirectory(ppmd_archiever)
add_subdirectory(numerical)
