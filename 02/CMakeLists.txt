cmake_minimum_required (VERSION 2.8.11)
project(02)
set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(02 main.cpp rasterizer.hpp rasterizer.cpp Triangle.hpp Triangle.cpp)

target_link_libraries(02 ${OpenCV_LIBRARIES} Eigen3::Eigen)