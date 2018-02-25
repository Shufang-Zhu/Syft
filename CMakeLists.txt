cmake_minimum_required(VERSION 2.8)
project(Syft)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
    "${CMAKE_MODULE_PATH}"
    "${CMAKE_ROOT_DIR}/cmake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

find_package(cudd REQUIRED)

add_executable(bdd Common.cpp DFA.cpp InputFirst.cpp main.cpp syn.cpp)

target_link_libraries(bdd ${CUDD_LIBRARIES})
