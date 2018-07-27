set(SRC main.cpp)
add_executable(moteur3D_controle ${SRC})
target_link_libraries(moteur3D_controle ${OPENSCENEGRAPH_LIBRARIES})

file(COPY data/ DESTINATION .)
