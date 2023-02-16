project(arithmetic_d_archiever)

add_executable(archiever_d_encoder encoder.cpp)
target_link_libraries(archiever_d_encoder arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(archiever_d_decoder decoder.cpp)
target_link_libraries(archiever_d_decoder arithmetic-encoding-lib common ${Boost_LIBRARIES})
