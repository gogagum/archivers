project(ppmd_archiever)

find_library(arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(ppmd_encoder encoder.cpp)
target_link_libraries(ppmd_encoder arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(ppmd_decoder decoder.cpp)
target_link_libraries(ppmd_decoder arithmetic-encoding-lib common ${Boost_LIBRARIES})