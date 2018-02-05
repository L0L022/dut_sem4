set(SRC main.cpp)
add_executable(moteur3D_tp3 ${SRC})
target_link_libraries(moteur3D_tp3 ${OPENSCENEGRAPH_LIBRARIES})

file(COPY data/ DESTINATION .)
