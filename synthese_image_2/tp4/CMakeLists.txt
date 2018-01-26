set(SRC main.cpp vector3f.cpp pgm.cpp terrain.cpp texture.cpp)
add_executable(synt_2_tp4 ${SRC})
target_link_libraries(synt_2_tp4 ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

configure_file(terrain_texture.png terrain_texture.png COPYONLY)
configure_file(terrain_128x128.pgm terrain_128x128.pgm COPYONLY)
