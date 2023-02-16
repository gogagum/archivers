project(a_archiever)

find_library(encoding_lib common ${Boost_LIBRARIES})

add_executable(archiever_a_encoder encoder.cpp)
target_link_libraries(archiever_a_encoder encoding_lib common ${Boost_LIBRARIES})

add_executable(archiever_a_decoder decoder.cpp)
target_link_libraries(archiever_a_decoder encoding_lib common ${Boost_LIBRARIES})