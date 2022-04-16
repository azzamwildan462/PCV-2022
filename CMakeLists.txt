cmake_minimum_required(VERSION 3.1)
project(PCV)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(PCV main.cpp)
target_link_libraries(PCV ${OpenCV_LIBS} ${YAML_CPP_LIBRARIES})