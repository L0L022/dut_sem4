#include <SuperOpenGL/SuperOpenGL.hpp>

#include <GL/freeglut.h>

void SuperOpenGL::init(int argc, char **argv)
{
    glutInit(&argc, argv);
}

void SuperOpenGL::mainLoop()
{
    glutMainLoop();
}
