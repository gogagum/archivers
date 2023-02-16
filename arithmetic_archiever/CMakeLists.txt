project(arithmetic_archiever)

find_library(encoding_lib common ${Boost_LIBRARIES})

add_executable(archiever_encoder encoder.cpp)
target_link_libraries(archiever_encoder encoding_lib common ${Boost_LIBRARIES})

add_executable(archiever_decoder decoder.cpp)
target_link_libraries(archiever_decoder encoding_lib common ${Boost_LIBRARIES})
