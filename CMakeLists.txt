cmake_minimum_required(VERSION 3.10)
project(FluxMemory VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(flux_memory INTERFACE)
target_include_directories(flux_memory INTERFACE include)

add_executable(flux_test src/main.cpp)
target_link_libraries(flux_test flux_memory)