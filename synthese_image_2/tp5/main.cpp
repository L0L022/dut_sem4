///////////////////////////////////////////////////////////////////////////////
// IUT d'Aix-Marseille Université, site d'Arles
// Département Informatique
// 2ème année
// ----------------------------------------------------------------------------
// Auteur : Sébastien Thon
// ----------------------------------------------------------------------------
// Programme permettant d'afficher un visage 3D.
///////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include "objet3d.h"

int SCREEN_WIDTH  = 800;								// Largeur de la fenêtre OpenGL
int SCREEN_HEIGHT = 600;								// Hauteur de la fenêtre OpenGl

#define KEY_ESC 27										// Code ASCII de la touche Echap


float	xpos = 0, ypos = 20, zpos = 150;				// Position de la caméra
int		angle_x = 0, angle_y = 0;						// Orientation de la caméra
int		mouse_x = 0, mouse_y = 0;						// Position de la souris

// Paramètres de la source de lumière 0 (directionnelle)
GLfloat Light0Position[]= { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat Light0Ambient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
GLfloat Light0Diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
GLfloat Light0Specular[]= { 0.5f, 0.5f, 0.5f, 1.0f };

Objet3D obj_visage, obj_normal, obj_colere, obj_degout, obj_joie, obj_peur, obj_surprise, obj_tristesse;
float valeur_interpolation = 0.f;

Objet3D * obj_from = &obj_visage, * obj_to = &obj_joie;

///////////////////////////////////////////////////////////////////////////////
// Intitialisation de certains paramètres d'OpenGL.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid initGL()
{
	glClearColor(0.7, 0.9, 1.0, 1);						// Couleur servant à effacer la fenêtre (bleu ciel)
    glShadeModel(GL_SMOOTH);							// Modèle d'ombrage : lissage de Gouraud
	glEnable(GL_CULL_FACE);								// Ne traite pas les faces cachées
	glEnable(GL_DEPTH_TEST);							// Active le Z-Buffer
	glDepthFunc(GL_LEQUAL);								// Mode de fonctionnement du Z-Buffer
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Active la correction de perspective (pour ombrage, texture, ...)

	glLightfv(GL_LIGHT0, GL_AMBIENT, Light0Ambient);	// Couleur ambiante de la source de lumière 0 (directionnelle)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Diffuse);	// Couleur diffuse de la source de lumière 0
	glLightfv(GL_LIGHT0, GL_SPECULAR,Light0Specular);	// Couleur spéculaire de la source de lumière 0
	glLightfv(GL_LIGHT0, GL_POSITION,Light0Position);	// Direction de la source de lumière 0
	glEnable(GL_LIGHT0);								// Activation de la source de lumière 0

    glEnable(GL_LIGHTING);								// Activation de l'éclairage général
}



///////////////////////////////////////////////////////////////////////////////
// Affiche les différents objets de la scène.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
void affiche_scene()
{
    obj_visage.affiche();
	glutSwapBuffers();							// Affiche la scène (affichage en double buffer)
}



///////////////////////////////////////////////////////////////////////////////
// Call-back : affichage dans la fenêtre.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_display()
{
	// On efface les pixels de l'image (color buffer) et le Z-Buffer (depth buffer).
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// On initialise la matrice de vue avec la matrice identité.
	glLoadIdentity();

	// On applique une translation et une rotation à la scène pour simuler
	// un déplacement de caméra.
	glTranslatef(-xpos,-ypos,-zpos);
	glRotatef(angle_x, 1,0,0);
	glRotatef(angle_y, 0,1,0);

	// On re-spécifie la position des sources de lumière avec pour qu'elles
	// soient aussi affectées par la translation et la rotation qu'on vient
	// d'appliquer à la scène.
    glLightfv(GL_LIGHT0, GL_POSITION,Light0Position);

	// On affiche la scène.
	affiche_scene();

	// On force OpenGL à afficher avant de passer à la suite.
	glFlush();
}



///////////////////////////////////////////////////////////////////////////////
// Call-back : redimensionnement de la fenêtre.
//-----------------------------------------------------------------------------
// Paramètres :
//    width, height (in) : nouvelles largeur et hauteur de la fenêtre.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,2000.0f);

	SCREEN_WIDTH  = width;
	SCREEN_HEIGHT = height;

	glMatrixMode(GL_MODELVIEW);
}

void update_visage() {
    obj_visage.interpolation(obj_from, obj_to, valeur_interpolation);
    glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// Call-back : gestion des événements clavier.
//-----------------------------------------------------------------------------
// Paramètres :
//    key (in) : code ascii de la touche utilisée.
//    x,y (in) : coordonnées de la souris.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_keyboard(unsigned char key, int x, int y)
{
    float step = 0.1;
	switch (key)
	{
		case KEY_ESC:						// 'ECHAP' :
			exit(1);						// on quitte le programme
			break;

    case '+':
        valeur_interpolation += step;
        if (valeur_interpolation > 1.f)
            valeur_interpolation = 1.f;
        std::cout << "valeur_interpolation = " << valeur_interpolation << std::endl;
        update_visage();
        break;
    case '-':
        valeur_interpolation -= step;
        if (valeur_interpolation < 0.f)
            valeur_interpolation = 0.f;
        std::cout << "valeur_interpolation = " << valeur_interpolation << std::endl;
        update_visage();
        break;

        case 'a':
            obj_from = &obj_colere;
            std::cout << "visage de colère" << std::endl;
            update_visage();
            break;
        case 'z':
            obj_from = &obj_degout;
            std::cout << "visage de dégout" << std::endl;
            update_visage();
            break;
        case 'e':
            obj_from = &obj_joie;
            std::cout << "visage de joie" << std::endl;
            update_visage();
            break;
        case 'r':
            obj_from = &obj_normal;
            std::cout << "visage de normal" << std::endl;
            update_visage();
            break;
        case 't':
            obj_from = &obj_peur;
            std::cout << "visage de peur" << std::endl;
            update_visage();
            break;
        case 'y':
            obj_from = &obj_surprise;
            std::cout << "visage de surprise" << std::endl;
            update_visage();
            break;
        case 'u':
            obj_from = &obj_tristesse;
            std::cout << "visage de tristesse" << std::endl;
            update_visage();
            break;

        case 'q':
            obj_to = &obj_colere;
            update_visage();
            std::cout << "visage à colère" << std::endl;
            break;
        case 's':
            obj_to = &obj_degout;
            std::cout << "visage à dégout" << std::endl;
            update_visage();
            break;
        case 'd':
            obj_to = &obj_joie;
            std::cout << "visage à joie" << std::endl;
            update_visage();
            break;
        case 'f':
            obj_to = &obj_normal;
            std::cout << "visage à normal" << std::endl;
            update_visage();
            break;
        case 'g':
            obj_to = &obj_peur;
            std::cout << "visage à peur" << std::endl;
            update_visage();
            break;
        case 'h':
            obj_to = &obj_surprise;
            std::cout << "visage à surprise" << std::endl;
            update_visage();
            break;
        case 'j':
            obj_to = &obj_tristesse;
            std::cout << "visage à tristesse" << std::endl;
            update_visage();
            break;
	}
}



///////////////////////////////////////////////////////////////////////////////
// Call-back : gestion des touches speciales du clavier.
//-----------------------------------------------------------------------------
// Paramètres :
//    key (in) : code ascii de la touche utilisée.
//    x,y (in) : coordonnées de la souris.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_special(int key, int x, int y)
{
	float speed = 1.0f;						// Vitesse de déplacement de la caméra

	switch (key)
	{
		case GLUT_KEY_UP:					// Flèche vers le haut : 
			zpos -= speed;					// on déplace la caméra selon z-
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_DOWN:					// Flèche vers le bas :
			zpos += speed;					// on déplace la caméra selon z+
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_LEFT:					// Flèche vers la gauche :
			xpos -= speed;					// on déplace la caméra selon x-
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_RIGHT:				// Flèche vers la droite :
			xpos += speed;					// on déplace la caméra selon x+
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_PAGE_UP:				// Page Up :
			ypos += speed;
			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_PAGE_DOWN:			// Page Down :
			ypos -= speed;
			glutPostRedisplay();			// et on demande le réaffichage.
			break;
	}
}



///////////////////////////////////////////////////////////////////////////////
// Call-back : gestion de clics souris.
//-----------------------------------------------------------------------------
// Paramètres :
//    button (in) : code du bouton utilisé.
//    state  (in) : état du bouton.
//    x,y    (in) : coordonnées de la souris.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		mouse_x = x;
		mouse_y = y;
	}
}



///////////////////////////////////////////////////////////////////////////////
// Call-back : gestion des déplacements de la souris.
//-----------------------------------------------------------------------------
// Paramètres :
//    x,y    (in) : coordonnées de la souris.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_motion(int x, int y)
{
	angle_x += y - mouse_y;						// Modifie la direction de vue de la caméra
	angle_y += x - mouse_x;						// en fonction de la position de la souris
	mouse_x = x;
	mouse_y = y;

	glutPostRedisplay();						// Demande le réaffichage
}



///////////////////////////////////////////////////////////////////////////////
// Initialise la scène.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
void initialise_scene()
{
    obj_normal.charge_OFF("visage_normal.off");
    obj_colere.charge_OFF("visage_colere.off");
    obj_degout.charge_OFF("visage_degout.off");
    obj_joie.charge_OFF("visage_joie.off");
    obj_peur.charge_OFF("visage_peur.off");
    obj_surprise.charge_OFF("visage_surprise.off");
    obj_tristesse.charge_OFF("visage_tristesse.off");
    obj_visage.charge_OFF("visage_normal.off");
}



///////////////////////////////////////////////////////////////////////////////
// Fonction principale du programme.
//-----------------------------------------------------------------------------
// Paramètres :
//    argc (in) : nombre d'arguments de la ligne de commande.
//    argv (in) : tableau de chaînes de caractères contenant les arguments.
// Retour :
//    Un entier contenant le code de retour du programme.
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	// Initialisation de paramètres de Glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("Visage");

	// Intitialisation de paramètres d'OpenGL
	initGL();

	// Initialisation de la scène
	initialise_scene();

	// Déclaration des fonctions de call-back.
	glutDisplayFunc(&callback_display);
	glutReshapeFunc(&callback_reshape);
	glutKeyboardFunc(&callback_keyboard);
	glutSpecialFunc(&callback_special);
	glutMouseFunc(&callback_mouse);
	glutMotionFunc(&callback_motion);

	// Boucle principale de Glut.
	glutMainLoop();

	// Code de retour
	return 1;
}
