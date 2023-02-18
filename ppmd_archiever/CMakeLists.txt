project(ppmd_archiever)

add_executable(ppmd_encoder encoder.cpp)
target_link_libraries(ppmd_encoder Boost::program_options common arithmetic-encoding-lib)

add_executable(ppmd_decoder decoder.cpp)
target_link_libraries(ppmd_decoder Boost::program_options common arithmetic-encoding-lib)
