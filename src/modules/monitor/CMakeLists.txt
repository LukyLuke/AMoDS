project(MonitorModules)
cmake_minimum_required (VERSION 2.8) 

add_library(echo SHARED echo.cpp)
target_link_libraries(echo ${PTHREAD})
