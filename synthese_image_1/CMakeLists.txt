cmake_minimum_required(VERSION 3.1)

project(synthese_image_1)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

add_subdirectory(SuperOpenGL)
add_subdirectory(tp1)
add_subdirectory(tp2)
add_subdirectory(tp3)
add_subdirectory(tp4)
