project(arithmetic_a_archiever)

find_library(arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(archiever_a_encoder encoder.cpp)
target_link_libraries(archiever_a_encoder arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(archiever_a_decoder decoder.cpp)
target_link_libraries(archiever_a_decoder arithmetic-encoding-lib common ${Boost_LIBRARIES})