include(FetchContent)

message(STATUS "FetchContent: fmt")

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG f6276a2c2b76c54c3a659d5fee5557c7bec95a0c
)

FetchContent_MakeAvailable(fmt)

message(STATUS "FetchContent: indicators")

FetchContent_Declare(indicators
    GIT_REPOSITORY https://github.com/p-ranav/indicators.git
    GIT_TAG 5050a8887786fe279a7ce47841cc805ef7e88326
)

FetchContent_MakeAvailable(indicators)

message(STATUS "FetchContent: arithmetic-encoding-lib")

FetchContent_Declare(arithmetic-encoding-lib
    GIT_REPOSITORY https://github.com/gogagum/arithmetic-encoding-lib.git
    GIT_TAG 74455369baf4105a179fa9044c9e69468b9c2d8e
)

FetchContent_MakeAvailable(arithmetic-encoding-lib)
