project(ppma_archiever)

add_executable(ppma_encoder encoder.cpp)
target_link_libraries(ppma_encoder archievers-applib arithmetic-encoding-lib)

add_executable(ppma_decoder decoder.cpp)
target_link_libraries(ppma_decoder archievers-applib arithmetic-encoding-lib)
