
add_library(dns SHARED dns.cpp)
target_link_libraries(dns ${PTHREAD})
