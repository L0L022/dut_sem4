set(SRC SuperOpenGL.cpp Vector.cpp Vertex.cpp Window.cpp)

add_library(SuperOpenGL STATIC ${SRC})
target_include_directories(SuperOpenGL SYSTEM PUBLIC ${INCLUDE_DIR})
target_link_libraries(SuperOpenGL ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
