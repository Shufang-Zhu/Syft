add_library(synthesis SHARED Common.cpp DFA.cpp InputFirst.cpp syn.cpp)

add_executable(bdd main.cpp)

target_link_libraries(bdd synthesis ${CUDD_LIBRARIES})
