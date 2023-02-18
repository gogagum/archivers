add_library(archievers-applib STATIC)

target_sources(archievers-applib
    PRIVATE
        src/exceptions.cpp
        src/file_opener.cpp
        src/opt_ostream.cpp
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
