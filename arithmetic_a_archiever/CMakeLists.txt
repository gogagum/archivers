project(arithmetic_a_archiever)

add_executable(archiever_a_encoder encoder.cpp)
target_link_libraries(archiever_a_encoder Boost::program_options common arithmetic-encoding-lib)

add_executable(archiever_a_decoder decoder.cpp)
target_link_libraries(archiever_a_decoder Boost::program_options common arithmetic-encoding-lib)
