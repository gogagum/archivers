include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG f6276a2c2b76c54c3a659d5fee5557c7bec95a0c
)

FetchContent_MakeAvailable(fmt)
