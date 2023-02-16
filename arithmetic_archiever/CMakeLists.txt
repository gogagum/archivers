project(arithmetic_archiever)

add_executable(archiever_encoder encoder.cpp)
target_link_libraries(archiever_encoder LINK_PUBLIC ${Boost_LIBRARIES} common arithmetic-encoding-lib)

add_executable(archiever_decoder decoder.cpp)
target_link_libraries(archiever_decoder LINK_PUBLIC ${Boost_LIBRARIES} common arithmetic-encoding-lib)
