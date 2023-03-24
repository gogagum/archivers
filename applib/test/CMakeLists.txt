include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(applib_tests
    bits_word_flow.cpp
    bits_word.cpp
    bytes_word_flow.cpp
    bytes_word.cpp
)

if (CMAKE_CROSSCOMPILING)
    message("Cross compiling. Use static libraries for tests.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
elseif()
    message("Not cross compiling. Use dynamic libraries.")
endif()
target_link_libraries(applib_tests LINK_PUBLIC gtest_main archievers-applib)

include(GoogleTest)
gtest_discover_tests(applib_tests)
