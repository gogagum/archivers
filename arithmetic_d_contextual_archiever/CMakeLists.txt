project(arithmetic_d_contextual_archiever)

add_executable(d_contextual_encoder encoder.cpp)
target_link_libraries(d_contextual_encoder archievers-applib arithmetic-encoding-lib)

add_executable(d_contextual_decoder decoder.cpp)
target_link_libraries(d_contextual_decoder archievers-applib arithmetic-encoding-lib)
