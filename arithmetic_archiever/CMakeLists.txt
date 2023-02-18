project(arithmetic_archiever)

add_executable(archiever_encoder encoder.cpp)
target_link_libraries(archiever_encoder Boost::program_options common arithmetic-encoding-lib)

add_executable(archiever_decoder decoder.cpp)
target_link_libraries(archiever_decoder Boost::program_options common arithmetic-encoding-lib)
