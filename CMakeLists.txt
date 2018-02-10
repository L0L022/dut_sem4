cmake_minimum_required(VERSION 3.1)

project(sem4_iut)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

find_package(OpenSceneGraph COMPONENTS osgDB osgViewer osgUtil osgText osgShadow osgParticle osgGA REQUIRED)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

add_subdirectory(synthese_image_1)
add_subdirectory(synthese_image_2)
add_subdirectory(moteurs_3D)
