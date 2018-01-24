set(SRC main.cpp)
add_executable(opengl_sfml ${SRC})
target_link_libraries(opengl_sfml ${SFML_LIBRARIES} ${OPENGL_LIBRARIES})
