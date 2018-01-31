set(SRC main.cpp)
add_executable(moteur3D_tp1 ${SRC})
target_link_libraries(moteur3D_tp1 ${OPENSCENEGRAPH_LIBRARIES})
