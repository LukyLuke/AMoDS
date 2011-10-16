project(libudp.so)
cmake_minimum_required (VERSION 2.8) 

add_library(udp SHARED udp.cpp)

target_link_libraries(udp ${PTHREAD})
