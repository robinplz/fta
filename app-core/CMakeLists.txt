cmake_minimum_required(VERSION 3.0.0)
project(app-core VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(app_core app-core.cpp)