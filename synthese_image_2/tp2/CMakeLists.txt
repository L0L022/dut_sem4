set(SRC main.cpp)
add_executable(synt_2_tp2 ${SRC})
target_link_libraries(synt_2_tp2 ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

configure_file(test.png test.png COPYONLY)
configure_file(facade.png facade.png COPYONLY)
configure_file(mur.png mur.png COPYONLY)
configure_file(toit.png toit.png COPYONLY)
configure_file(sol.png sol.png COPYONLY)
