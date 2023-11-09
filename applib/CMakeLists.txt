add_library(archievers-applib STATIC)

add_subdirectory(thirdparty)

find_package(fmt)
find_package(indicators)

target_sources(archievers-applib
    PRIVATE
        src/decode_impl.cpp
        src/exceptions.cpp
        src/file_opener.cpp
        src/ord_and_tail_splitter.cpp
        src/log_stream_get.cpp
)

target_include_directories(archievers-applib
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(archievers-applib arithmetic-encoding-lib boost_program_options fmt::fmt indicators::indicators)

if (BUILD_TEST)
    add_subdirectory(test)
endif (BUILD_TEST)
