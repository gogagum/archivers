project(ppmd_archiever)

add_executable(ppmd_encoder encoder.cpp)
target_link_libraries(ppmd_encoder archievers-applib arithmetic-encoding-lib)

add_executable(ppmd_decoder decoder.cpp)
target_link_libraries(ppmd_decoder archievers-applib arithmetic-encoding-lib)
