cmake_minimum_required(VERSION 3.5)
project(Syftmax)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
#set(CMAKE_CXX_FLAGS_RELEASE "-O0")

set(CMAKE_MODULE_PATH
    "${CMAKE_MODULE_PATH}"
    "${CMAKE_ROOT_DIR}/cmake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


find_package(cudd REQUIRED)
find_package(mona REQUIRED)

set(LYDIA_DIR ${PROJECT_SOURCE_DIR}/submodules/lydia)
set(WITH_SYFT OFF)
add_subdirectory(${LYDIA_DIR})
set(LYDIA_LIBRARIES ${LYDIA_DIR})

set(LYDIA_INCLUDE_DIR ${LYDIA_DIR}/lib/include)
set(LYDIA_THIRD_PARTY_INCLUDE_PATH "${LYDIA_DIR}/third_party/spdlog/include;${LYDIA_DIR}/third_party/CLI11/include;/usr/local/include;/usr/local/include;/usr/local/include;${LYDIA_DIR}/third_party/google/benchmark/include;/usr/include")

set(EXT_LIBRARIES_PATH lydia ${CUDD_LIBRARIES} ${MONA_DFA_LIBRARIES} ${MONA_BDD_LIBRARIES} ${MONA_MEM_LIBRARIES})
set(EXT_INCLUDE_PATH ${LYDIA_INCLUDE_DIR} ${LYDIA_THIRD_PARTY_INCLUDE_PATH} ${CUDD_INCLUDE_DIRS} ${MONA_MEM_INCLUDE_DIRS} ${MONA_BDD_INCLUDE_DIRS} ${MONA_DFA_INCLUDE_DIRS})

message(STATUS EXT_LIBRARIES_PATH ${EXT_LIBRARIES_PATH})
add_subdirectory(src)


