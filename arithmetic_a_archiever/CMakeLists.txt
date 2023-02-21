project(arithmetic_a_archiever)

add_executable(archiever_a_encoder encoder.cpp)
target_link_libraries(archiever_a_encoder archievers-applib arithmetic-encoding-lib)

add_executable(archiever_a_decoder decoder.cpp)
target_link_libraries(archiever_a_decoder archievers-applib arithmetic-encoding-lib)
