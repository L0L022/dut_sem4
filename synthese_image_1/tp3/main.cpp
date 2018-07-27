//2016 R. RAFFIN, IUT Aix-Marseille, dept. Informatique - Arles
//TP simple, affichages de quelques points
//Loïc Escales

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <functional>
#include <vector>

#include <SuperOpenGL/Window.hpp>
#include <SuperOpenGL/Vector3.hpp>

#define KEY_ESC 27

namespace SOGL = SuperOpenGL;

struct Planet {
    SOGL::Vector3f position;
    SOGL::Vector3f color;
    float size = 1.f;
    float mySpeed = 0.f;
    float otherSpeed = 0.f;
    float myAngle = 0.f;
    float otherAngle = 0.f;
    std::vector<std::reference_wrapper<Planet>> planets;

    void show() {
        glPushMatrix();
            glRotatef(otherAngle, 0.0, 1.0, 0.0);
            glTranslatef(position.x, position.y, position.z);

            glPushMatrix();
                glRotatef(myAngle, 0.0, 1.0, 0.0);
                glColor3f(color.x, color.y, color.z);
                glutSolidSphere(size, 10, 10);
            glPopMatrix();

            for (Planet & p : planets)
                    p.show();

        glPopMatrix();
    }

    void update() {
        myAngle += mySpeed;
        otherAngle += otherSpeed;

        for (Planet & p : planets)
                p.update();
    }
};

class MainWindow : public SOGL::Window {
public:
    MainWindow()
        : SOGL::Window("TP3 : evenements, hierarchies")
        , _moveStep(0.5f)
        , _lookDistance(5.f)
        , _mouseAngleX(0.f)
        , _mouseAngleY(0.f)
        , _oldMouseX(width()/2)
        , _oldMouseY(height()/2)
        , _sun{{0, 0, 0}, {1.0, 1.0, 0.0}, 4.f, 1.f}
        , _earth{{10, 0, 0}, {0.0, 0.0, 1.0}, 2.f, _sun.mySpeed * 3.f, 1.f}
        , _moon{{4, 0, 0}, {0.7, 0.7, 0.7}, 1.f, _sun.mySpeed * 1.5f, 2.f * _earth.mySpeed}
        , _mars{{18, 0, 0}, {1.0, 0.0, 0.0}, 1.8f, _sun.mySpeed, _earth.otherSpeed}
    {
        _sun.planets.push_back(_earth);
        _earth.planets.push_back(_moon);
        _sun.planets.push_back(_mars);
    }

protected:
    void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Modification de la matrice de projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity(); //remise à 0 (identité)
        gluPerspective(130.0, width()/float(height()), 0.1, 100); //définition d'une perspective (angle d'ouverture 130°,rapport L/H=1.0, near=0.1, far=100)

        //Modification de la matrice de modélisation de la scène
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //Définition de la position de l'observateur
        gluLookAt(_lookDistance, _lookDistance, _lookDistance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //paramètres position(5.0, 5.0, 5.0), point visé(0.0, 0.0, 0.0), upVector - verticale (0.0, 1.0, 0.0)

        //rotation due aux mouvements de la souris
        glRotatef(_mouseAngleX, 1.0, 0.0, 0.0);
        glRotatef(_mouseAngleY, 0.0, 1.0, 0.0);

        //dessin des axes du repère
        drawAxis();

        _sun.show();

        glutSwapBuffers();
    }

    void keyPressed(unsigned char key, int x, int y) {
        switch (key) {
        case KEY_ESC:
            glutLeaveMainLoop();
            break;
        }
    }

    void specialKeyPressed(int key, int x, int y) {
        switch (key) {
        case GLUT_KEY_UP :
            _lookDistance -= _moveStep;
            break;
        case GLUT_KEY_DOWN :
            _lookDistance += _moveStep;
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
        _sun.update();
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

private:
    const float _moveStep;
    float _lookDistance;
    float _mouseAngleX;
    float _mouseAngleY;
    int _oldMouseX;
    int _oldMouseY;
    Planet _sun;
    Planet _earth;
    Planet _moon;
    Planet _mars;
};

static void InitializeGL() {
    GLfloat lightPosition [] = { 0.0f, 10.0f, 0.0f, 0.0 };

    //Crée une source de lumière directionnelle
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //Définit un modèle d'ombrage
    glShadeModel(GL_SMOOTH);

    //Z Buffer pour la suppression des parties cachées
    glEnable(GL_DEPTH_TEST);

    //La variable d'état de couleur GL_AMBIENT_AND_DIFFUSE
    //est définie par des appels à glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

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

    glutMainLoop();

	return (EXIT_SUCCESS);
}
