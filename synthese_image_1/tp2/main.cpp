//2016 R. RAFFIN, IUT Aix-Marseille, dept. Informatique - Arles
//TP simple, affichages de quelques points

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <memory>

#include <SuperOpenGL/Window.hpp>
#include <SuperOpenGL/Vector3.hpp>
#include <SuperOpenGL/RegularTetrahedronGeometry.hpp>

namespace SOGL = SuperOpenGL;

class MainWindow : public SOGL::Window {
public:
    MainWindow()
        : SOGL::Window("TP2 : couleurs, transformations, caméras")
        , _angle(0.f)
        , _isOrtho(true)
        , _showTeapot(true)
        , _orthoAngle(0)
        , _orthoVec(0, 1, 0)
    {}

    void init() {
        initVertexBuffer();
        initColorBuffer();
    }

protected:
    void display() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (_isOrtho) {
            glOrtho(-1.f, 1.f, -1.f, 1.f, -100.f, 100.f);
            glRotatef(_orthoAngle, _orthoVec.x, _orthoVec.y, _orthoVec.z);
        } else {
            gluPerspective(45.f, width()/float(height()), 0.1f, 100.f);
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (!_isOrtho)
            gluLookAt(0.0, 0.0, 5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        if (_showTeapot) {
            glColor3f(0.2, 0.5, 0.3);
            glutWireTeapot(0.5);
        } else {
            glColor3f(1.0, 0.0, 0.0);

            glRotatef(_angle, 0, 1, 0);

            glEnableClientState(GL_VERTEX_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
            glVertexPointer(3, GL_FLOAT, 0, 0);

            glEnableClientState(GL_COLOR_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
            glColorPointer(3, GL_FLOAT, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, _vertexBufferSize);

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
        }

        glutSwapBuffers();
    }

    void keyPressed(unsigned char key, int x, int y) {
        switch (key) {
        case 'v':
            _isOrtho = !_isOrtho;
            break;
        case 't':
            _showTeapot = !_showTeapot;
            break;
        case 'a':
            _orthoAngle = 90.f;
            _orthoVec = {0, 1, 0};
            break;
        case 'z':
            _orthoAngle = 45.f;
            _orthoVec = {0, 1, 0};
            break;
        case 'e':
            _orthoAngle = 90.f;
            _orthoVec = {1, 0, 0};
            break;
        default:
            break;
        }
    }

    void update(const float elaspedTime) {
        _angle += 1.f;
    }

    void reshape(size_t width, size_t height) {
        glViewport(0, 0, width, height);
    }

private:
    void initVertexBuffer() {
        SOGL::RegularTetrahedronGeometry geo({0.f, 0.f, 0.f}, 0.5);
        _vertexBufferSize = geo.verticesCount();

        glGenBuffers(1, &_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _vertexBufferSize, geo.vertices(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initColorBuffer() {
        std::vector<SOGL::Vector3f> colors{{1.f,    0.f,    0.f},
                                           {0.f,    1.f,    0.f},
                                           {0.f,    0.f,    1.f},
                                           {1.f,    0.f,    0.f},
                                           {0.f,    1.f,    0.f},
                                           {0.f,    0.f,    1.f},
                                           {1.f,    0.f,    0.f},
                                           {0.f,    1.f,    0.f},
                                           {0.f,    0.f,    1.f},
                                           {1.f,    0.f,    0.f},
                                           {0.f,    1.f,    0.f},
                                           {0.f,    0.f,    1.f}};

        glGenBuffers(1, &_colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * colors.size(), colors.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    GLuint _vertexBuffer;
    size_t _vertexBufferSize;
    GLuint _colorBuffer;

    float _angle;
    bool _isOrtho;
    bool _showTeapot;
    float _orthoAngle;
    SOGL::Vector3f _orthoVec;
};

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    MainWindow window;
    window.setPosition(200, 200);
    window.resize(500, 500);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
        return (EXIT_FAILURE);
    }

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    window.init();

    glutMainLoop();

	return (EXIT_SUCCESS);
}
