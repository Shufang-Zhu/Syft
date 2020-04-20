cmake_minimum_required(VERSION 3.5)
project(Syft)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_BUILD_TYPE Debug)

add_compile_options(-std=c++14)

set(CMAKE_MODULE_PATH
    "${CMAKE_MODULE_PATH}"
    "${CMAKE_ROOT_DIR}/cmake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
  
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
  
find_package(cudd REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

add_subdirectory(src)
