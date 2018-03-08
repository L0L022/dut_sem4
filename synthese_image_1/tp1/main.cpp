//2016 R. RAFFIN, IUT Aix-Marseille, dept. Informatique - Arles
//TP simple, affichages de quelques points

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <memory>

#include <SuperOpenGL/Window.hpp>
#include <SuperOpenGL/Vector3.hpp>
#include <SuperOpenGL/CircleGeometry.hpp>

namespace SOGL = SuperOpenGL;

class MainWindow : public SOGL::Window {
public:
    MainWindow()
        : SOGL::Window("TP1 : quelques points")
    {}

    void init() {
        glGenBuffers(1, &_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        initCircle();
//        initRandom();
//        initStar();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

protected:
    void display() {
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glDrawArrays(GL_POINTS, 0, _verticesCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableClientState(GL_VERTEX_ARRAY);

        glutSwapBuffers();
    }

private:
    void initCircle() {
        _verticesCount = 200;

        SOGL::CircleGeometry circle({0.5f, 0.5f, 0.f}, 0.5f, _verticesCount);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * circle.verticesCount(), circle.vertices(), GL_STATIC_DRAW);
    }

    void initRandom() {
        _verticesCount = 200;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.f, 1.f);

        auto vertices = std::make_unique<float[]>(_verticesCount * 3);

        for (size_t i = 0; i < _verticesCount * 3; ++i)
            vertices[i] = dis(gen);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _verticesCount, vertices.get(), GL_STATIC_DRAW);
    }

    void initStar() {
        std::vector<SOGL::Vector3f> vertices{{0.f,    0.f,    0.f},
                                             {0.25f,  0.25f,  0.f},
                                             {0.f,    0.9f,   0.f},
                                             {-0.25f, 0.25f,  0.f},
                                             {-0.9f,  0.f,    0.f},
                                             {-0.25f, -0.25f, 0.f},
                                             {0.f,    -0.9f,  0.f},
                                             {0.25f,  -0.25f, 0.f},
                                             {0.9f,   0.f,    0.f}};

        _verticesCount = vertices.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertices.size(), reinterpret_cast<const float*>(vertices.data()), GL_STATIC_DRAW);
    }

private:
    GLuint _VBO;
    size_t _verticesCount;
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

    window.init();

    glutMainLoop();

	return (EXIT_SUCCESS);
}
