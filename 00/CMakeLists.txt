cmake_minimum_required (VERSION 2.8.11)
project(00)
set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
add_executable(00 main.cpp)
target_link_libraries(00 Eigen3::Eigen)