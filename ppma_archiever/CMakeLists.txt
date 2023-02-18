project(ppma_archiever)

add_executable(ppma_encoder encoder.cpp)
target_link_libraries(ppma_encoder Boost::program_options common arithmetic-encoding-lib)

add_executable(ppma_decoder decoder.cpp)
target_link_libraries(ppma_decoder Boost::program_options common arithmetic-encoding-lib)
