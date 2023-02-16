project(numerical_archiever)

find_library(arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(numerical_encoder encoder.cpp)
target_link_libraries(numerical_encoder arithmetic-encoding-lib common ${Boost_LIBRARIES})

add_executable(numerical_decoder decoder.cpp)
target_link_libraries(numerical_decoder arithmetic-encoding-lib common ${Boost_LIBRARIES})