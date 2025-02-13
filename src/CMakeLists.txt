add_subdirectory(synthesis)


include_directories(${SYNTHESIS_INCLUDE_PATH} ${EXT_INCLUDE_PATH})
add_executable(Syftmax Main.cpp)

target_link_libraries(Syftmax ${SYNTHESIS_LIB_NAME})

install(TARGETS Syftmax
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

INSTALL (
  DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")
