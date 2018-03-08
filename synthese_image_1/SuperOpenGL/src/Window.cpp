#include <SuperOpenGL/Window.hpp>

#include <GL/freeglut.h>

namespace SuperOpenGL {

std::map<int, Window *> Window::_instances;

Window::Window(const std::string &title)
    : _id(-1), _posX(0), _posY(0), _width(100), _height(100), _title(title), _updateFreq(20.f), _elapsedTime(0), _lastTimer(std::chrono::system_clock::now())
{
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowPosition(_posX, _posY);
    glutInitWindowSize(_width, _height);
    _id = glutCreateWindow(_title.c_str());
    _instances[_id] = this;

    glutDisplayFunc(&Window::displayCallback);
    glutReshapeFunc(&Window::reshapeCallback);
    glutKeyboardFunc(&Window::keyboardCallback);
    glutSpecialFunc(&Window::specialCallback);
    glutTimerFunc(_updateFreq, &Window::timerCallback, 0);
}

Window::~Window()
{
    _instances.erase(_id);
}

void Window::setTitle(const std::string &title)
{
    _title = title;
    glutSetWindow(_id);
    glutSetWindowTitle(_title.c_str());
}

void Window::setWidth(const size_t width)
{
    resize(width, _height);
}

void Window::setHeigth(const size_t height)
{
    resize(_width, height);
}

void Window::resize(const size_t width, const size_t height)
{
    _width = width;
    _height = height;
    glutSetWindow(_id);
    glutReshapeWindow(_width, _height);
}

void Window::setPosition(const size_t x, const size_t y)
{
    _posX = x;
    _posY = y;
    glutSetWindow(_id);
    glutPositionWindow(_posX, _posY);
}

void Window::displayCallback()
{
    _instances[glutGetWindow()]->display();
}

void Window::reshapeCallback(int width, int height)
{
    _instances[glutGetWindow()]->reshape(width, height);
}

void Window::keyboardCallback(unsigned char key, int x, int y)
{
    _instances[glutGetWindow()]->keyPressed(key, x, y);
}

void Window::specialCallback(int key, int x, int y)
{
    _instances[glutGetWindow()]->specialKeyPressed(key, x, y);
}

void Window::timerCallback(int)
{
    _instances[glutGetWindow()]->timer();
}

void Window::timer()
{
    auto currentTimer = std::chrono::system_clock::now();
    _elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>(currentTimer - _lastTimer).count();
    _lastTimer = currentTimer;
    while (_elapsedTime > _updateFreq) {
        _elapsedTime -= _updateFreq;
        update(_updateFreq);
    }
    glutPostRedisplay();
    glutTimerFunc(_updateFreq, &Window::timerCallback, 0);
}

}
