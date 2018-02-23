#include <SuperOpenGL/Window.hpp>

#include <GL/freeglut.h>

std::map<int, SuperOpenGL::Window *> SuperOpenGL::Window::_instances;

SuperOpenGL::Window::Window(const std::string &title)
    : _id(-1), _posX(0), _posY(0), _width(100), _height(100), _title(title)
{
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(_posX, _posY);
    glutInitWindowSize(_width, _height);
    _id = glutCreateWindow(_title.c_str());
    _instances[_id] = this;

    glutDisplayFunc(&Window::displayCallback);
}

void SuperOpenGL::Window::setTitle(const std::string &title)
{
    _title = title;
    glutSetWindow(_id);
    glutSetWindowTitle(_title.c_str());
}

void SuperOpenGL::Window::setWidth(const size_t width)
{
    resize(width, _height);
}

void SuperOpenGL::Window::setHeigth(const size_t height)
{
    resize(_width, height);
}

void SuperOpenGL::Window::resize(const size_t width, const size_t height)
{
    _width = width;
    _height = height;
    glutSetWindow(_id);
    glutReshapeWindow(_width, _height);
}

void SuperOpenGL::Window::setPosition(const size_t x, const size_t y)
{
    _posX = x;
    _posY = y;
    glutSetWindow(_id);
    glutPositionWindow(_posX, _posY);
}

void SuperOpenGL::Window::displayCallback()
{
    _instances[glutGetWindow()]->display();
}
