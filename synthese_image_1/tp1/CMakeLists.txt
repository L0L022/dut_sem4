set(SRC main.cpp)
add_executable(synt_1_tp1 ${SRC})
target_link_libraries(synt_1_tp1 ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
