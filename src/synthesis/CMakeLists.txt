set (CMAKE_CXX_STANDARD 14)

add_library(synthesis SHARED Common.cpp DFA.cpp InputFirst.cpp syn.cpp)
include_directories(${CUDD_INCLUDE_DIRS})

#add_executable(synth synStrategy.cpp)
add_executable(Syft main.cpp)

target_link_libraries(Syft synthesis ${CUDD_LIBRARIES})
#target_link_libraries(synth synthesis ${CUDD_LIBRARIES})

#install(TARGETS bdd synth synthesis
install(TARGETS Syft synthesis
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

INSTALL (
  DIRECTORY ${CMAKE_SOURCE_DIR}/src/synthesis
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")
