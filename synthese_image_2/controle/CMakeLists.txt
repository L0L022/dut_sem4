set(SRC main.cpp vector3f.cpp)
add_executable(synt_2_controle ${SRC})
target_link_libraries(synt_2_controle ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

file(COPY data/ DESTINATION .)
