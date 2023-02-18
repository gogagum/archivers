project(arithmetic_d_archiever)

add_executable(archiever_d_encoder encoder.cpp)
target_link_libraries(archiever_d_encoder Boost::program_options arithmetic-encoding-lib common)

add_executable(archiever_d_decoder decoder.cpp)
target_link_libraries(archiever_d_decoder Boost::program_options arithmetic-encoding-lib common)
