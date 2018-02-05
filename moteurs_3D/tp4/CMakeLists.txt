set(SRC main.cpp)
add_executable(moteur3D_tp4 ${SRC})
target_link_libraries(moteur3D_tp4 ${OPENSCENEGRAPH_LIBRARIES})

file(COPY data/ DESTINATION .)
