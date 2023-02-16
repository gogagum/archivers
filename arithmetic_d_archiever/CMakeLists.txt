project(arithmetic_d_archiever)

find_library(encoding_lib common ${Boost_LIBRARIES})

add_executable(archiever_d_encoder encoder.cpp)
target_link_libraries(archiever_d_encoder encoding_lib common ${Boost_LIBRARIES})

add_executable(archiever_d_decoder decoder.cpp)
target_link_libraries(archiever_d_decoder encoding_lib common ${Boost_LIBRARIES})