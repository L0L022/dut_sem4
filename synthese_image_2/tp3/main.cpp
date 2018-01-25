///////////////////////////////////////////////////////////////////////////////
// IUT d'Aix-Marseille Université, site d'Arles
// Département Informatique
// 2ème année
// ----------------------------------------------------------------------------
// IN - Synthèse d'images - Rendu réaliste
// Auteur : Sébastien Thon
// ----------------------------------------------------------------------------
// Base du TP 1
// Base d'un programme permettant d'afficher une scène composée d'un sol, de
// maisons et d'arbres. On placera des sources de lumière, on spécifiera les
// matériaux des différents objets et on utilisera un effet de brouillard.
// ----------------------------------------------------------------------------
// Compilation sous Windows :
//   g++ -Wall maison.cpp -o maison.exe -lfreeglut -lglu32 -lopengl32
// Compilation sous Linux :
//   g++ -Wall maison.cpp -o maison.exe -lfreeglut -lGLU -lGL
///////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <string>
#include <memory>
#include <iostream>
#include <list>
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

float	xpos = 0, ypos = 0, zpos = -50;							// Position de la caméra
int		angle_x = 45, angle_y = 45;						// Orientation de la caméra
int		mouse_x = 0, mouse_y = 0;						// Position de la souris

GLfloat lumiere_pos[] = {0.0, 5.0, 0.0, 1.0};
GLfloat lumiere_color[] = {1, 1, 1};

//*****************************************************************
//* A FAIRE :
//* Déclarer des tableaux contenant les caractéristiques (Ambient,
//* diffus, spéculaire) pour différents matériaux :
//*    - sol
//*    - mur de maison
//*    - toit de maison
//*    - tronc d'arbre
//*    - feuillage d'arbre
//*
//* Par exemple, pour le matériau de sol:
//*   GLfloat MatAmbient_sol[] = {0.0f, 0.5f, 0.0f, 1.0f};	// On utilise des valeurs faibles pour Ambient (sombre, car à l'ombre)
//*   GLfloat MatDiffuse_sol[] = {0.0f, 0.7f, 0.0f, 1.0f};	// On utilise des valeurs plus élevées pour Diffuse (partie éclairée)
//*   GLfloat MatSpecular_sol[]= {0.1f, 0.1f, 0.1f, 1.0f};	// Plus ces valeurs seront élevées, et plus le matériau sera luisant
//*   GLfloat MatShininess_sol[1]={5.0f};					// Plus cette valeurs era élevée (max=128) et plus le reflet sera petit
//*****************************************************************

void setColor(uint8_t r, uint8_t g, uint8_t b)
{
    GLfloat MatDiffuse[4] = {r/255.f, g/255.f, b/255.f, 1.0f};
    GLfloat MatAmbient[4] = {MatDiffuse[0]/2.f, MatDiffuse[1]/2.f, MatDiffuse[2]/2.f, 1.0f};
    GLfloat MatSpecular[4] = {0.2f,0.2f, 0.2f, 1.0f};
    GLfloat MatShininess[1] = { 5.0F };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDiffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, MatShininess);
}

void setColorWhite() { setColor(255, 255, 255); }
void setColorBlack() { setColor(0, 0 ,0); }
void setColorRed() { setColor(255, 0, 0); }
void setColorGreen() { setColor(0, 255, 0); }
void setColorBlue() { setColor(0, 0, 255); }

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
        std::cout << id << "\n";

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
Texture texture_mur;
Texture texture_facade;
Texture texture_toit;
Texture texture_fumee;

class Bouffee {
private:
    Vector3f _position;
    Vector3f _vitesse;
    float _taille;
    float _vie;

public:
    Bouffee(Vector3f position, Vector3f vitesse, float taille, float vie) : _position(position), _vitesse(vitesse), _taille(taille), _vie(vie) {
    }

    void anime(float temps) {
        _position += _vitesse * temps;
        _vie -= temps;
        _taille += temps * 1.f;
    }

    void affiche() const {
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glPushMatrix();
        //glTranslatef(_position.x, _position.y, _position.z);
//        std::cout << _position.y << std::endl;

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

        A.x = xpos + Haut.x - Droite.x;
        A.y = ypos + Haut.y - Droite.y;
        A.z = zpos + Haut.z - Droite.z;
        B.x = xpos + Haut.x + Droite.x;
        B.y = ypos + Haut.y + Droite.y;
        B.z = zpos + Haut.z + Droite.z;
        C.x = xpos - Haut.x + Droite.x;
        C.y = ypos - Haut.y + Droite.y;
        C.z = zpos - Haut.z + Droite.z;

        D.x = xpos - Haut.x - Droite.x;
        D.y = ypos - Haut.y - Droite.y;
        D.z = zpos - Haut.z - Droite.z;

        Vector3f vertices[4] = {A, B, C, D};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_QUADS);									// Affichage d'un quadrilatère
            glTexCoord2f(0.0f,0.0f);
            vertices[0].glVertex();
            glTexCoord2f(0.0f,1.0f);
            vertices[1].glVertex();
            glTexCoord2f(1.0f,1.0f);
            vertices[2].glVertex();
            glTexCoord2f(1.0f,0.0f);
            vertices[3].glVertex();
        glEnd();

        glPopMatrix();

        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
    }

    inline float vie() const { return _vie; }
};

class Fumee {
private:
    Vector3f _position;
    std::list<Bouffee> _liste_bouffee;
    float _intervale_emission;
    float _temps_ecoule;
    Texture * _bouffee_text;

public:
    Fumee(Vector3f position, float intervale_emission, Texture * text) : _position(position), _intervale_emission(intervale_emission), _temps_ecoule(0.f), _bouffee_text(text) {

    }

    void anime(float temps) {
        _temps_ecoule += temps;
        while(_temps_ecoule >= _intervale_emission) {
            _liste_bouffee.emplace_back(_position, Vector3f{0.f, 1.f, 0.f}, 1.f, 10.f);

            _liste_bouffee.erase(std::remove_if(_liste_bouffee.begin(), _liste_bouffee.end(), [](const Bouffee &b){ return b.vie() <= 0; }), _liste_bouffee.end());



            _temps_ecoule -= _intervale_emission;
        }

        for (Bouffee &b : _liste_bouffee) {
            b.anime(temps);
        }
    }

    void affiche() const {
        glDepthMask(GL_FALSE);
        _bouffee_text->utiliser();
        for (const Bouffee &b : _liste_bouffee) {
            b.affiche();
        }
        glDepthMask(GL_TRUE);
    }
};

Fumee fumee(Vector3f(0, 0, 0), 1, &texture_fumee);

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
	glClearColor(0, 0, 0, 1);							// Couleur servant à effacer la fenêtre (noir)
    glShadeModel(GL_SMOOTH);							// Modèle d'ombrage : lissage de Gouraud
	glEnable(GL_CULL_FACE);								// Ne traite pas les faces cachées
	glEnable(GL_DEPTH_TEST);							// Active le Z-Buffer
	glDepthFunc(GL_LEQUAL);								// Mode de fonctionnement du Z-Buffer
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Active la correction de perspective (pour ombrage, texture, ...)


	//*****************************************************************
	//* A FAIRE :
	//*
	//* 1. Créer la source de lumière 0 (GL_LIGHT0) en tant que source de
	//*    lumière directionnelle, en précisant :
	//*      - sa position
	//*      - sa couleur ambiante
	//*      - sa couleur diffuse
	//*      - sa couleur spéculaire
	//*
	//* 2. Activer la source de lumière 0.
	//*
	//* 3. Activer l'éclairage général
	//*****************************************************************

    {
    //GLfloat direction[] = {0.0, 5.0, 0.0, 0.0};
    GLfloat amb[] = {0.5, 0.5, 0.5, 1};
    GLfloat dif[] = {1, 1, 1, 1};
    GLfloat spec[] = {0.2, 0.2, 0.2, 1};

    //glLightfv(GL_LIGHT0, GL_POSITION, direction);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glEnable(GL_LIGHT0);
    }
    {
    //GLfloat direction[] = {0.0, 5.0, 0.0, 0.0};
    GLfloat amb[] = {0.5, 0, 0, 1};
    GLfloat dif[] = {1, 0, 0, 1};
    GLfloat spec[] = {0.2, 0.2, 0.2, 1};

    //glLightfv(GL_LIGHT0, GL_POSITION, direction);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);

    glEnable(GL_LIGHT1);
    }

    {
        GLfloat fogColor[4]= {0.4f,0.4f,0.4f,0.0f};
        glFogf(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 20);
        // défaut : 0.0f
        glFogf(GL_FOG_END, 50);
        // défaut : 1.0f
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogf(GL_FOG_DENSITY, 1.f);
        //glEnable(GL_FOG);
    }

    glEnable(GL_LIGHTING);

    texture_sol.charger("test.png");
    texture_mur.charger("mur.png");
    texture_facade.charger("facade.png");
    texture_toit.charger("toit.png");
    texture_fumee.charger("fumee.tga");
    glEnable(GL_TEXTURE_2D);
}



///////////////////////////////////////////////////////////////////////////////
// Affiche le sol de la scène.
//-----------------------------------------------------------------------------
// Paramètres :
//    _
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
void affiche_sol()
{
	//*****************************************************************
	//* A FAIRE :
	//* Remplacer le glColor3f() suivant par l'utilisation du matériau
	//* crée pour le sol (utiliser glMaterialfv() pour les paramètres
	//* ambiant, diffus, speculaire du matériau du sol).
	//*****************************************************************
    glEnable(GL_TEXTURE_2D);
    texture_sol.utiliser();

    Vector3f vertices[4] = {{-20, 0, -20}, {-20, 0, 20}, {20, 0, 20}, {20, 0, -20}};
    ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
	glBegin(GL_QUADS);									// Affichage d'un quadrilatère
        glTexCoord2f(0.0f,0.0f);
        vertices[0].glVertex();
        glTexCoord2f(0.0f,10.0f);
        vertices[1].glVertex();
        glTexCoord2f(10.0f,10.0f);
        vertices[2].glVertex();
        glTexCoord2f(10.0f,0.0f);
        vertices[3].glVertex();
	glEnd();
}



///////////////////////////////////////////////////////////////////////////////
// Affiche une maison.
//-----------------------------------------------------------------------------
// Paramètres :
//    xp,yp,zp (in) : position de la maison.
//    yr       (in) : angle de rotation (en degrés) de la maison p/r à l'axe y.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
void affiche_maison( float xp, float yp, float zp, float yr )
{
	glPushMatrix();										// Sauve la matrice de vue actuelle
	glTranslatef(xp, yp, zp);							// Positionne la maison avec une translation
	glRotatef(yr, 0,1,0);								// et une rotation


	//*****************************************************************
	//* A FAIRE :
	//* Associer une normale à chacun des polygones suivants. Pour cela
	//* faire appel à glNormal3f() avant chacun des "glBegin(GL_QUADS)"
	//* et des "glBegin(GL_TRIANGLES)".
	//*****************************************************************

	//*****************************************************************
	//* A FAIRE :
	//* Remplacer le glColor3f() suivant par l'utilisation du matériau
	//* crée pour les murs (utiliser glMaterialfv() pour les paramètres
	//* ambiant, diffus, speculaire du matériau du mur).
	//*****************************************************************
    setColorWhite();

	// Mur de face
    {
        Vector3f vertices[4] = {{-4, 5, 5}, {-4, 0, 5}, { 4, 0, 5}, { 4, 5, 5}};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_QUADS);
            for(int i = 0; i < 4; ++i)
                vertices[i].glVertex();
        glEnd();
    }

	// Mur arrière
    {
        Vector3f vertices[4] = {{4, 5, -5}, {4, 0, -5}, { -4, 0, -5}, { -4, 5, -5}};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_QUADS);
            for(int i = 0; i < 4; ++i)
                vertices[i].glVertex();
        glEnd();
    }

	// Mur gauche
    {
        Vector3f vertices[4] = {{-4, 5, -5}, {-4, 0, -5}, {-4, 0,  5}, {-4, 5,  5}};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_QUADS);
            for(int i = 0; i < 4; ++i)
                vertices[i].glVertex();
        glEnd();
    }


	// Mur droit
    {
        Vector3f vertices[4] = {{4, 5,  5}, {4, 0,  5}, {4, 0, -5}, {4, 5, -5}};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_QUADS);
            for(int i = 0; i < 4; ++i)
                vertices[i].glVertex();
        glEnd();
    }

	// Triangle avant
    {
        Vector3f vertices[3] = {{0, 9, 5}, {-4, 5, 5}, {4, 5, 5}};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_TRIANGLES);
            for(int i = 0; i < 3; ++i)
                vertices[i].glVertex();
        glEnd();
    }

	// Triangle arrière
    {
        Vector3f vertices[3] = {{0, 9, -5}, {4, 5, -5}, {-4, 5, -5}};
        ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).normalize().glNormal();
        glBegin(GL_TRIANGLES);
            for(int i = 0; i < 3; ++i)
                vertices[i].glVertex();
        glEnd();
    }

//*****************************************************************
//* A FAIRE :
//* Remplacer le glColor3f() suivant par l'utilisation du matériau
//* crée pour le toit (utiliser glMaterialfv() pour les paramètres
//* ambiant, diffus, speculaire du matériau du toit).
//*****************************************************************
    setColorRed();

	// Toit versant droit
    Vector3f vertices_d[4] = {{0, 9,  5}, {4, 5,  5}, {4, 5, -5}, {0, 9, -5}};
    ((vertices_d[1] - vertices_d[0]) ^ (vertices_d[2] - vertices_d[0])).normalize().glNormal();
    glBegin(GL_QUADS);									// Affichage d'un quadrilatère
        for(int i = 0; i < 4; ++i)
            vertices_d[i].glVertex();
    glEnd();

	// Toit versant gauche
    Vector3f vertices_g[4] = {{ 0, 9, -5}, {-4, 5, -5}, {-4, 5,  5}, { 0, 9,  5}};
    ((vertices_g[1] - vertices_g[0]) ^ (vertices_g[2] - vertices_g[0])).normalize().glNormal();
    glBegin(GL_QUADS);									// Affichage d'un quadrilatère
        for(int i = 0; i < 4; ++i)
            vertices_g[i].glVertex();
    glEnd();


	glPopMatrix();										// Restaure la matrice de vue
}



///////////////////////////////////////////////////////////////////////////////
// Affiche un arbre.
//-----------------------------------------------------------------------------
// Paramètres :
//    xp,yp,zp (in) : position de l'arbre.
// Retour :
//    _
///////////////////////////////////////////////////////////////////////////////
void affiche_arbre( float xp, float yp, float zp )
{
	glPushMatrix();										// Sauve la matrice de vue actuelle
	glTranslatef(xp, yp, zp);							// Positionne l'arbre avec une translation

	//*****************************************************************
	//* A FAIRE :
	//* Afficher un arbre dont le tronc sera un cône :
	//*     glutSolidCone()
	//* et le feuillage une sphère :
	//*     glutSolidSphere()
	//* Préciser le matériau pour chacun de ces éléments.
	//*****************************************************************

    float height = 5;

    glPushMatrix();
    setColor(160, 82, 45);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(1, height, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, height, 0);
    setColorGreen();
    glutSolidSphere(height/2.f, 10, 10);
    glPopMatrix();

	glPopMatrix();										// Restaure la matrice de vue
}

void affiche_lumiere()
{
    setColorWhite();
    glLightfv(GL_LIGHT1, GL_POSITION, lumiere_pos);
//    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lumiere_color);

    glPushMatrix();
    glTranslatef(lumiere_pos[0], lumiere_pos[1], lumiere_pos[2]);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();
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
	affiche_sol();								// On affiche le sol.

	//*****************************************************************
	//* A FAIRE :
	//* Afficher quelques maisons et quelques arbres.
	//*
	//* Par exemple, on affiche ici une maison en (0,0,0),
	//* tournée de 20° :
	//*****************************************************************
    affiche_maison( 0, 0, -10, 20 );
    affiche_maison( 10, 0, 0, 50 );
    affiche_maison( -10, 0, 0, -20 );
    affiche_maison( 0, 0, 10, 0 );

    affiche_arbre(15, 0, 10);
    affiche_arbre(15, 0, -10);

    affiche_lumiere();

    fumee.affiche();

	glutSwapBuffers();							// Affiche la scène à l'écran (affichage en double buffer)
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
    float dt = mesure_temps_ecoule();

	// On efface les pixels de l'image (color buffer) et le Z-Buffer (depth buffer).
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// On initialise la matrice de vue avec la matrice identité.
	glLoadIdentity();

	// On applique une translation et une rotation à la scène pour simuler
	// un déplacement de caméra.
	glTranslatef(xpos,0,zpos);
	glRotatef(angle_x, 1,0,0);
	glRotatef(angle_y, 0,1,0);

	//*****************************************************************
	//* A FAIRE :
	//* Re-spécifier la position des sources de lumière avec glLightfv()
	//* pour qu'elles soient aussi affectées par la translation et la
	//* rotation qu'on vient d'appliquer à la scène.
	//*****************************************************************
    {
        GLfloat direction[] = {5.0, 5.0, 5.0, 0.0};
        glLightfv(GL_LIGHT0, GL_POSITION, direction);
    }

    fumee.anime(dt);

	// On affiche la scène.
    affiche_scene();

	// On force OpenGL à afficher avant de passer à la suite.
	glFlush();
}



///////////////////////////////////////////////////////////////////////////////
// Call-back : fonction appelée lorsqu'on redimensionne la fenêtre.
//-----------------------------------------------------------------------------
// Paramètres :
//    width, height (in) : nouvelles largeur et hauteur de la fenêtre.
// Retour :
//    _glutSolidCone
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);

	glMatrixMode(GL_MODELVIEW);
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
    float speed = 1.0f;
    float speed_lum = 0.05f;

	switch (key)
	{
		case KEY_ESC:						// 'ECHAP' :
			exit(1);						// on quitte le programme
			break;
        case 'z':					// Flèche vers le haut :
            lumiere_pos[2] += speed;					// on déplace la caméra selon z-
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;

        case 's':					// Flèche vers le bas :
            lumiere_pos[2] -= speed;					// on déplace la caméra selon z+
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;

        case 'q':					// Flèche vers la gauche :
            lumiere_pos[0] += speed;					// on déplace la caméra selon x-
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;

        case 'd':				// Flèche vers la droite :
            lumiere_pos[0] -= speed;					// on déplace la caméra selon x+
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;
        case 'r':				// Flèche vers la droite :
            lumiere_pos[1] += speed;					// on déplace la caméra selon x+
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;
        case 'f':				// Flèche vers la droite :
            lumiere_pos[1] -= speed;					// on déplace la caméra selon x+
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;
        case 'w':				// Flèche vers la droite :
            lumiere_color[0] -= speed_lum;					// on déplace la caméra selon x+
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;
        case 'x':				// Flèche vers la droite :
            lumiere_color[0] += speed_lum;					// on déplace la caméra selon x+
//            glutPostRedisplay();			// et on demande le réaffichage.
            break;
    case 'c':				// Flèche vers la droite :
        lumiere_color[1] -= speed_lum;					// on déplace la caméra selon x+
//        glutPostRedisplay();			// et on demande le réaffichage.
        break;
    case 'v':				// Flèche vers la droite :
        lumiere_color[1] += speed_lum;					// on déplace la caméra selon x+
//        glutPostRedisplay();			// et on demande le réaffichage.
        break;
    case 'b':				// Flèche vers la droite :
        lumiere_color[2] -= speed_lum;					// on déplace la caméra selon x+
//        glutPostRedisplay();			// et on demande le réaffichage.
        break;
    case 'n':				// Flèche vers la droite :
        lumiere_color[2] += speed_lum;					// on déplace la caméra selon x+
//        glutPostRedisplay();			// et on demande le réaffichage.
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
			zpos += speed;					// on déplace la caméra selon z-
//			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_DOWN:					// Flèche vers le bas :
			zpos -= speed;					// on déplace la caméra selon z+
//			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_LEFT:					// Flèche vers la gauche :
			xpos += speed;					// on déplace la caméra selon x-
//			glutPostRedisplay();			// et on demande le réaffichage.
			break;

		case GLUT_KEY_RIGHT:				// Flèche vers la droite :
			xpos -= speed;					// on déplace la caméra selon x+
//			glutPostRedisplay();			// et on demande le réaffichage.
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


GLvoid timer(int v) {
    glutPostRedisplay();
    glutTimerFunc(20, timer, 0);
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
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Maison");

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
