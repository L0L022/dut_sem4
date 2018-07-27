//2016 R. RAFFIN, IUT Aix-Marseille, dept. Informatique - Arles
//TP simple, affichages de quelques points
//Loïc Escales

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <functional>
#include <vector>
#include <cmath>
#include <memory>

#include <SuperOpenGL/Window.hpp>
#include <SuperOpenGL/Vector3.hpp>

#define KEY_ESC 27

int factorial(int n) {
    if (n == 0)
        return 1;

    return n == 1 ? n : n * factorial(n - 1);
}

namespace SOGL = SuperOpenGL;

class ParametricEquation {
public:
    virtual SOGL::Vector3f operator()(const float t) const = 0;
};

class BezierCurve : public ParametricEquation {
public:
    using ControlPoints = std::vector<SOGL::Vector3f>;

    ControlPoints controlPoints;
};

class BernsteinBasedBezierCurve : public BezierCurve {
public:
    virtual SOGL::Vector3f operator()(const float t) const {
        SOGL::Vector3f r;
        for (size_t i = 0; i < controlPoints.size(); ++i)
            r += controlPoints[i] * bernstein(i, controlPoints.size() - 1, t);
        return r;
    }

private:
    static float bernstein(const float i, const float n, const float t) {
        return factorial(n) / float(factorial(i) * factorial(n - i)) * std::pow(t, i) * std::pow(1 - t, n - i);
    }
};

class DeCasteljauBasedBezierCurve : public BezierCurve {
public:
    virtual SOGL::Vector3f operator()(const float t) const {
        auto points = controlPoints;
        while (points.size() > 1) {
            ControlPoints nPoints;
            nPoints.reserve(points.size() - 1);
            for (size_t i = 0; i < points.size() - 1; ++i)
                nPoints.push_back(points[i] * t + points[i + 1] * (1 - t));
            points = nPoints;
        }
        return points[0];
    }
};

class MainWindow : public SOGL::Window {
public:
    MainWindow()
        : SOGL::Window("TP4 : Courbe(s) de Bezier")
        , _moveStep(0.5f)
        , _lookDistance(5.f)
        , _mouseAngleX(0.f)
        , _mouseAngleY(0.f)
        , _oldMouseX(width()/2)
        , _oldMouseY(height()/2)
        , _bezierCurve(new BernsteinBasedBezierCurve)
        , _currentControlPoint(0)
    {
        _bezierCurve->controlPoints = {{-2.0, -2.0, 0.0},
                                       {-1.0,  1.0, 0.0},
                                       { 1.0,  1.0, 0.0},
                                       { 2.0, -2.0, 0.0}};
    }

    void init() {
        glGenBuffers(1, &_vertexBufferCP);
        glGenBuffers(1, &_colorBufferCP);
        glGenBuffers(1, &_vertexBufferD);

        updateVertexBufferCP();
        updateColorBufferCP();
        discretization();
    }

protected:
    void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Modification de la matrice de projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity(); //remise à 0 (identité)
        glOrtho(-_lookDistance, _lookDistance, -_lookDistance, _lookDistance, -_lookDistance, _lookDistance);

        //Modification de la matrice de modélisation de la scène
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //rotation due aux mouvements de la souris
        glRotatef(_mouseAngleX, 1.0, 0.0, 0.0);
        glRotatef(_mouseAngleY, 0.0, 1.0, 0.0);

        //dessin des axes du repère
        drawAxis();

        glColor3f(1.0, 1.0, 1.0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferD);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_LINE_STRIP, 0, _vertexBufferSizeD);

        //dessin du polygone de contrôle
        glColor3f(0.4, 0.4, 0.4);

        //Liaison avec le buffer de vertex
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferCP);
        glVertexPointer(3, GL_FLOAT, 0, 0); //description des données pointées
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_LINE_STRIP, 0, _vertexBufferSizeCP); //les éléments à utiliser pour le dessin

        //dessin des points de contrôle
        //avec le même tableau de donnes (le VBO)

        glEnableClientState(GL_COLOR_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, _colorBufferCP);
        glColorPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //on modifie la taille d'un point pour plus de "joliesse"
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, _vertexBufferSizeCP); //les éléments à utiliser pour le dessin

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glutSwapBuffers();
    }

    void keyPressed(unsigned char key, int x, int y) {
        switch (key) {
        case KEY_ESC:
            glutLeaveMainLoop();
            break;
        case '+':
            if (_currentControlPoint + 1 < _bezierCurve->controlPoints.size()) {
                ++_currentControlPoint;
                updateColorBufferCP();
            }
            break;
        case '-':
            if (_currentControlPoint > 0) {
                --_currentControlPoint;
                updateColorBufferCP();
            }
            break;
        case 'a':
            _lookDistance -= _moveStep;
            break;
        case 'q':
            _lookDistance += _moveStep;
            break;
        }
    }

    void specialKeyPressed(int key, int x, int y) {
        const float step = 0.5;
        switch (key) {
        case GLUT_KEY_PAGE_UP:
            moveCurrentCP({0.0, 0.0, step});
            break;
        case GLUT_KEY_PAGE_DOWN:
            moveCurrentCP({0.0, 0.0, -step});
            break;
        case GLUT_KEY_UP:
            moveCurrentCP({0.0, step, 0.0});
            break;
        case GLUT_KEY_DOWN:
            moveCurrentCP({0.0, -step, 0.0});
            break;
        case GLUT_KEY_LEFT:
            moveCurrentCP({-step, 0.0, 0.0});
            break;
        case GLUT_KEY_RIGHT:
            moveCurrentCP({step, 0.0, 0.0});
            break;
        }
    }

    void mouseMoved(int x, int y) {
        //rotation de la scene suivant les mouvements de la souris
        int dx = _oldMouseX - x;
        int dy = _oldMouseY - y;

        _oldMouseX = x;
        _oldMouseY = y;

        _mouseAngleX += dy;
        _mouseAngleY += dx;
    }

    void update(const float elaspedTime) {
    }

private:
    void drawAxis() {
        glBegin(GL_LINES);
            //Ox, rouge
            glColor3f(1.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(8.0, 0.0, 0.0);

            //Oy, vert
            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 8.0, 0.0);

            //Oz, bleu
            glColor3f(0.0, 0.0, 1.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, 8.0);

        glEnd();
    }

    void updateVertexBufferCP() {
        _vertexBufferSizeCP = _bezierCurve->controlPoints.size();
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferCP);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _vertexBufferSizeCP, _bezierCurve->controlPoints.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void updateColorBufferCP() {
        std::vector<SOGL::Vector3f> colors(_bezierCurve->controlPoints.size(), {0.8, 0.8, 0.3});
        colors[_currentControlPoint] = {0.0, 0.0, 1.0};

        glBindBuffer(GL_ARRAY_BUFFER, _colorBufferCP);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * colors.size(), colors.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void moveCurrentCP(const SOGL::Vector3f &dir) {
        _bezierCurve->controlPoints[_currentControlPoint] += dir;
        updateVertexBufferCP();
        discretization();
    }

    void discretization() {
        const size_t nbP = 10;

        std::vector<SOGL::Vector3f> vertices;
        vertices.reserve(nbP);

        float t = 0.f;
        for (size_t i = 0; i < nbP; ++i) {
            vertices.push_back((*_bezierCurve)(t));
            t += 1/float(nbP-1);
        }

        _vertexBufferSizeD = vertices.size();
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferD);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _vertexBufferSizeD, vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    const float _moveStep;
    float _lookDistance;
    float _mouseAngleX;
    float _mouseAngleY;
    int _oldMouseX;
    int _oldMouseY;

    GLuint _vertexBufferCP;
    size_t _vertexBufferSizeCP;

    GLuint _colorBufferCP;

    GLuint _vertexBufferD;
    size_t _vertexBufferSizeD;

    std::unique_ptr<BezierCurve> _bezierCurve;
    size_t _currentControlPoint;
};

static void InitializeGL() {
    //Z Buffer pour la suppression des parties cachées
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

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

    InitializeGL();
    window.init();

    glutMainLoop();

	return (EXIT_SUCCESS);
}
