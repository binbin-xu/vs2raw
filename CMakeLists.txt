cmake_minimum_required(VERSION 2.8.12)
project(vs2Raw)

add_definitions(-std=c++11)

find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    add_definitions(-DOpenCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS})
  link_libraries(${OpenCV_LIBRARIES})
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(vs2Raw
    raw.cpp
    visim2binraw.cpp
    string_tools.cpp
    file_tools.cpp)

