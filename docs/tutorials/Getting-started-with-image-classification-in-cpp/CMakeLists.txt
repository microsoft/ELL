cmake_minimum_required(VERSION 2.8)
project(tutorial)
set(CMAKE_CXX_STANDARD 11)

# Change this to set OpenCV_DIR to your opencv/build location
set(OpenCV_DIR C:/Install/OpenCV3/opencv/build)

add_subdirectory(model)

find_package(OpenCV REQUIRED)
add_executable(tutorial tutorial.cpp)
target_link_libraries(tutorial ${OpenCV_LIBS} model)
