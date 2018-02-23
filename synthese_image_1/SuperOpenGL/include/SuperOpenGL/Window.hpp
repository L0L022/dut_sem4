#ifndef SUPEROPENGL_WINDOW_HPP
#define SUPEROPENGL_WINDOW_HPP

#include <iostream>
#include <string>
#include <map>

namespace SuperOpenGL {

class Window {
public:
    Window(const std::string &title);

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

private:
    static void displayCallback();

private:
    static std::map<int, Window *> _instances;
    int _id;
    size_t _posX, _posY;
    size_t _width, _height;
    std::string _title;
};

}

#endif
