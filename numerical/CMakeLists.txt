project(numerical_archiever)

add_executable(numerical_encoder encoder.cpp)
target_link_libraries(numerical_encoder archievers-applib arithmetic-encoding-lib)

add_executable(numerical_decoder decoder.cpp)
target_link_libraries(numerical_decoder archievers-applib arithmetic-encoding-lib)