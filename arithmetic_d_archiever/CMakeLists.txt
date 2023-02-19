project(arithmetic_d_archiever)

add_executable(archiever_d_encoder encoder.cpp)
target_link_libraries(archiever_d_encoder archievers-applib arithmetic-encoding-lib)

add_executable(archiever_d_decoder decoder.cpp)
target_link_libraries(archiever_d_decoder archievers-applib arithmetic-encoding-lib)
