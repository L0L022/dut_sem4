#ifndef SUPEROPENGL_WINDOW_HPP
#define SUPEROPENGL_WINDOW_HPP

#include <iostream>
#include <string>
#include <map>
#include <chrono>

namespace SuperOpenGL {

class Window {
public:
    Window(const std::string &title);
    virtual ~Window();

    inline std::string title() const { return _title; }
    void setTitle(const std::string &title);

    inline size_t width() const { return _width; }
    void setWidth(const size_t width);

    inline size_t height() const { return _height; }
    void setHeigth(const size_t height);

    void resize(const size_t width, const size_t height);

    inline size_t posX() const { return _posX; }
    inline size_t posY() const { return _posY; }
    void setPosition(const size_t x, const size_t y);

protected:
    virtual void display() {}
    virtual void reshape(size_t width, size_t height) {}
    virtual void keyPressed(unsigned char key, int x, int y) {}
    virtual void specialKeyPressed(int key, int x, int y) {}
    virtual void update(const float elaspedTime) {}

private:
    static void displayCallback();
    static void reshapeCallback(int width, int height);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialCallback(int key, int x, int y);
    static void timerCallback(int);

    void timer();

private:
    static std::map<int, Window *> _instances;
    int _id;
    size_t _posX, _posY;
    size_t _width, _height;
    std::string _title;
    float _updateFreq;
    float _elapsedTime;
    std::chrono::system_clock::time_point _lastTimer;
};

}

#endif
