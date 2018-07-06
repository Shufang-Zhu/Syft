add_library(synthesis SHARED Common.cpp DFA.cpp InputFirst.cpp syn.cpp)

add_executable(synth synStrategy.cpp)
add_executable(bdd main.cpp)

target_link_libraries(bdd synthesis ${CUDD_LIBRARIES})
target_link_libraries(synth synthesis ${CUDD_LIBRARIES})

install(TARGETS bdd synth
  RUNTIME DESTINATION bin
  )
