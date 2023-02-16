project(numerical_archiever)

find_library(encoding_lib common ${Boost_LIBRARIES})

add_executable(numerical_encoder encoder.cpp)
target_link_libraries(numerical_encoder encoding_lib common ${Boost_LIBRARIES})

add_executable(numerical_decoder decoder.cpp)
target_link_libraries(numerical_decoder encoding_lib common ${Boost_LIBRARIES})