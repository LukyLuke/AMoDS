project(amods)
cmake_minimum_required (VERSION 2.8) 

find_library(PTHREAD pthread)

add_executable(amods src/factory.cpp src/module.cpp src/hypercube.cpp src/helper/math.cpp src/helper/random.cpp src/helper/guid.cpp src/thread.cpp src/amods.cpp main.cpp)
set_target_properties(amods PROPERTIES ENABLE_EXPORTS ON)
target_link_libraries(amods ${PTHREAD} dl)

add_subdirectory(src)
