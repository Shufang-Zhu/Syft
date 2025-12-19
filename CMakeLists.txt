cmake_minimum_required(VERSION 3.5)
project(SpotSyft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_MODULE_PATH
    "${CMAKE_MODULE_PATH}"
    "${CMAKE_ROOT_DIR}/cmake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


find_package(cudd REQUIRED)
#find_package(mona REQUIRED)
find_package(spot REQUIRED)
find_package(buddy REQUIRED)

#set(LYDIA_DIR ${PROJECT_SOURCE_DIR}/submodules/lydia)
#set(WITH_SYFT OFF)
#add_subdirectory(${LYDIA_DIR})
#set(LYDIA_LIBRARIES ${LYDIA_DIR})

#set(LYDIA_INCLUDE_DIR ${LYDIA_DIR}/lib/include)
#set(LYDIA_THIRD_PARTY_INCLUDE_PATH "${LYDIA_DIR}/third_party/spdlog/include;${LYDIA_DIR}/third_party/CLI11/include;/usr/local/include;/usr/local/include;/usr/local/include;${LYDIA_DIR}/third_party/google/benchmark/include;/usr/include")

set(EXT_LIBRARIES_PATH ${SPOT_LIBRARIES} ${BUDDY_LIBRARIES} ${CUDD_LIBRARIES})
set(EXT_INCLUDE_PATH ${SPOT_INCLUDE_DIRS} ${BUDDY_INCLUDE_DIRS} ${CUDD_INCLUDE_DIRS})

message(STATUS EXT_LIBRARIES_PATH ${EXT_LIBRARIES_PATH})
add_subdirectory(src)


