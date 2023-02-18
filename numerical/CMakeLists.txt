project(numerical_archiever)

add_executable(numerical_encoder encoder.cpp)
target_link_libraries(numerical_encoder Boost::program_options arithmetic-encoding-lib common)

add_executable(numerical_decoder decoder.cpp)
target_link_libraries(numerical_decoder Boost::program_options arithmetic-encoding-lib common)