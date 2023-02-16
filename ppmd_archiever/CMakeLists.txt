project(ppmd_archiever)

find_library(encoding_lib common ${Boost_LIBRARIES})

add_executable(ppmd_encoder encoder.cpp)
target_link_libraries(ppmd_encoder encoding_lib common ${Boost_LIBRARIES})

add_executable(ppmd_decoder decoder.cpp)
target_link_libraries(ppmd_decoder encoding_lib common ${Boost_LIBRARIES})