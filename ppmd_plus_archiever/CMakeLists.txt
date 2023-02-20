project(ppmd_plus_archiever)

add_executable(ppmd_plus_encoder encoder.cpp)
target_link_libraries(ppmd_plus_encoder archievers-applib arithmetic-encoding-lib)

add_executable(ppmd_plus_decoder decoder.cpp)
target_link_libraries(ppmd_plus_decoder archievers-applib arithmetic-encoding-lib)
