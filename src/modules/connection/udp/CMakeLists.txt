
add_library(udp SHARED udp.cpp)
target_link_libraries(udp ${PTHREAD})
