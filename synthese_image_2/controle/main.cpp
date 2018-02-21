#include <cmath>
#include <memory>
#include <string>
#include <list>
#include <random>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "vector3f.h"

#define WIDTH   800										// Largeur de la fenêtre OpenGL
#define HEIGHT  600										// Hauteur de la fenêtre OpenGl

#define KEY_ESC 27										// Code ASCII de la touche Echap

float	xpos = 0, zpos = -50;							// Position de la caméra
int		angle_x = 45, angle_y = 45;						// Orientation de la caméra
int		mouse_x = 0, mouse_y = 0;						// Position de la souris

class Texture {
private:
    GLuint id;
    int width, height;
    std::unique_ptr<stbi_uc[]> img;

public:
    Texture() : id(0), width(0), height(0) {

    }

    bool charger(const std::string &fileName)
    {
        glGenTextures(1, &id);
        definir_filtrage(GL_NEAREST, GL_NEAREST);
        definir_bouclage(GL_REPEAT, GL_REPEAT);
        definir_melange(GL_MODULATE);

        int comp;
        img.reset(stbi_load(fileName.c_str(), &width, &height, &comp, 0));
        if (img == nullptr)
            return false;

        GLenum format;
        switch (comp) {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            return false;
        }
        utiliser();
        glTexImage2D(GL_TEXTURE_2D, 0, comp, width, height, 0, format, GL_UNSIGNED_BYTE, img.get());
        return true;
    }

    void utiliser() {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void definir_filtrage(GLint mode_min, GLint mode_mag) {
        utiliser();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode_min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode_mag);
    }

    void definir_bouclage(GLint mode_axe_s, GLint mode_axe_t) {
        utiliser();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_axe_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_axe_t);
    }

    void definir_melange(GLint mode) {
        utiliser();
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
    }
};

Texture texture_sol;

class Flocon {
public:
    Flocon(const Vector3f &pos, const Vector3f &vitesse, const float taille) : _position(pos), _vitesse(vitesse), _taille(taille) {}

    void anime(float temps) {
        _position += _vitesse * temps;
    }

    void affiche() const {
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        float t = _taille / 2.f;

        float matrice [16];
        glGetFloatv(GL_MODELVIEW_MATRIX, matrice);
        Vector3f Haut, Droite;
        Droite.x = matrice[0] * t;
        Droite.y = matrice[4] * t;
        Droite.z = matrice[8] * t;
        Haut.x = matrice[1] * t;
        Haut.y = matrice[5] * t;
        Haut.z = matrice[9] * t;

        Vector3f A, B, C, D;

        A.x = _position.x + Haut.x - Droite.x;
        A.y = _position.y + Haut.y - Droite.y;
        A.z = _position.z + Haut.z - Droite.z;
        B.x = _position.x + Haut.x + Droite.x;
        B.y = _position.y + Haut.y + Droite.y;
        B.z = _position.z + Haut.z + Droite.z;
        C.x = _position.x - Haut.x + Droite.x;
        C.y = _position.y - Haut.y + Droite.y;
        C.z = _position.z - Haut.z + Droite.z;
        D.x = _position.x - Haut.x - Droite.x;
        D.y = _position.y - Haut.y - Droite.y;
        D.z = _position.z - Haut.z - Droite.z;

        Vector3f vertices[4] = {A, B, C, D};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            vertices[0].glVertex();
            glTexCoord2f(0.0f,1.0f);
            vertices[1].glVertex();
            glTexCoord2f(1.0f,1.0f);
            vertices[2].glVertex();
            glTexCoord2f(1.0f,0.0f);
            vertices[3].glVertex();
        glEnd();

        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
    }

    inline Vector3f position() const { return _position; }

private:
    Vector3f _position;
    Vector3f _vitesse;
    float _taille;
};

class Neige {
public:

    Neige(Vector3f pos_min, Vector3f pos_max, float intervale_emission) : disX(pos_min.x, pos_max.x), disY(pos_min.y, pos_max.y), disZ(pos_min.z, pos_max.z), _intervalle_emission(intervale_emission), _temps_ecoule(0) {
        std::random_device rd;
        gen.seed(rd());
    }

    void charche_texture() {
        _texture_flocon.charger("particule.png");
    }

    void anime(float temps) {
        _temps_ecoule += temps;

        while (_temps_ecoule >= _intervalle_emission) {
            _temps_ecoule -= _intervalle_emission;

            _flocons.push_back(Flocon({disX(gen), disY(gen), disZ(gen)}, {0, -1, 0}, 1));
        }

        _flocons.erase(std::remove_if(_flocons.begin(), _flocons.end(), [this](const Flocon &f){ return f.position().y < disY.min(); }), _flocons.end());

        for (auto &f : _flocons)
            f.anime(temps);
    }

    void affiche() {
        _texture_flocon.utiliser();
        for (auto &f : _flocons)
            f.affiche();
    }

private:
    std::mt19937 gen;
    std::uniform_real_distribution<float> disX, disY, disZ;
    std::list<Flocon> _flocons;
    Texture _texture_flocon;
    float _intervalle_emission;
    float _temps_ecoule;
};

Neige neige({-25, 0, -25}, {25, 50, 25}, 0.5);

GLvoid initGL()
{
	glClearColor(0, 0, 0, 1);							// Couleur servant à effacer la fenêtre (noir)
    glShadeModel(GL_SMOOTH);							// Modèle d'ombrage : lissage de Gouraud
	glEnable(GL_CULL_FACE);								// Ne traite pas les faces cachées
	glEnable(GL_DEPTH_TEST);							// Active le Z-Buffer
	glDepthFunc(GL_LEQUAL);								// Mode de fonctionnement du Z-Buffer
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Active la correction de perspective (pour ombrage, texture, ...)

    GLfloat amb[] = {0.5, 0.5, 0.5, 1};
    GLfloat dif[] = {1, 1, 1, 1};
    GLfloat spec[] = {0.2, 0.2, 0.2, 1};
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    texture_sol.charger("neige.tga");
    neige.charche_texture();
    glEnable(GL_TEXTURE_2D);
}

void affiche_sol()
{
    texture_sol.utiliser();

    Vector3f vertices[4] = {{-25, 0, -25}, {-25, 0, 25}, {25, 0, 25}, {25, 0, -25}};
    ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f,0.0f);
    vertices[0].glVertex();
    glTexCoord2f(0.0f,5.0f);
    vertices[1].glVertex();
    glTexCoord2f(5.0f,5.0f);
    vertices[2].glVertex();
    glTexCoord2f(5.0f,0.0f);
    vertices[3].glVertex();

	glEnd();
}

void affiche_scene()
{
    affiche_sol();
    neige.affiche();

    glutSwapBuffers();
}

float mesure_temps_ecoule()
{
    static long temps_precedent = -1;
    long temps_actuel;
    float dt = 0;
    temps_actuel = glutGet(GLUT_ELAPSED_TIME);

    if (temps_precedent == -1) {
        temps_precedent = glutGet(GLUT_ELAPSED_TIME);
        dt = 0;
    } else {
        dt = (float)(temps_actuel-temps_precedent)/1000.0f;
        temps_precedent = temps_actuel;
    }
    return dt;
}

GLvoid timer(int v) {
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
}

GLvoid callback_display()
{
    float dt = mesure_temps_ecoule();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(xpos,0,zpos);
	glRotatef(angle_x, 1,0,0);
	glRotatef(angle_y, 0,1,0);

    GLfloat direction[] = {0, 1, 0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, direction);

    neige.anime(dt);

    affiche_scene();

	glFlush();
}

GLvoid callback_reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);

	glMatrixMode(GL_MODELVIEW);
}

GLvoid callback_keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case KEY_ESC:						// 'ECHAP' :
			exit(1);						// on quitte le programme
			break;
	}
}

GLvoid callback_special(int key, int x, int y)
{
	float speed = 1.0f;						// Vitesse de déplacement de la caméra

	switch (key)
	{
		case GLUT_KEY_UP:					// Flèche vers le haut : 
			zpos += speed;					// on déplace la caméra selon z-
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_DOWN:					// Flèche vers le bas :
			zpos -= speed;					// on déplace la caméra selon z+
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_LEFT:					// Flèche vers la gauche :
			xpos += speed;					// on déplace la caméra selon x-
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_RIGHT:				// Flèche vers la droite :
			xpos -= speed;					// on déplace la caméra selon x+
			glutPostRedisplay();			// et on demande le réaffichage.
			break;
	}
}

GLvoid callback_mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		mouse_x = x;
		mouse_y = y;
	}
}

GLvoid callback_motion(int x, int y)
{
	angle_x += y - mouse_y;						// Modifie la direction de vue de la caméra
	angle_y += x - mouse_x;						// en fonction de la position de la souris
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();						// Demande le réaffichage
}

int main(int argc, char *argv[])
{
	// Initialisation de paramètres de Glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("Controle");

	// Intitialisation de paramètres d'OpenGL
	initGL();

	// Déclaration des fonctions de call-back.
	glutDisplayFunc(&callback_display);
	glutReshapeFunc(&callback_reshape);
	glutKeyboardFunc(&callback_keyboard);
	glutSpecialFunc(&callback_special);
	glutMouseFunc(&callback_mouse);
	glutMotionFunc(&callback_motion);
    glutTimerFunc(20, timer, 0);

	// Boucle principale de Glut.
	glutMainLoop();

	// Code de retour
	return 1;
}
