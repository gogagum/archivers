project(arithmetic_archiever)

add_executable(archiever_encoder encoder.cpp)
target_link_libraries(archiever_encoder archievers-applib arithmetic-encoding-lib)

add_executable(archiever_decoder decoder.cpp)
target_link_libraries(archiever_decoder archievers-applib arithmetic-encoding-lib)
