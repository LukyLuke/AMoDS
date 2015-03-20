
add_library(echo SHARED echo.cpp)
target_link_libraries(echo ${PTHREAD})
