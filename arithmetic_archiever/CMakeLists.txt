project(arithmetic_archiever)

find_library(arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(archiever_encoder encoder.cpp)
target_link_libraries(archiever_encoder arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(archiever_decoder decoder.cpp)
target_link_libraries(archiever_decoder arithmetic-encoding-lib common ${Boost_LIBRARIES})
