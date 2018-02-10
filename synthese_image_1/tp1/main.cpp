//2016 R. RAFFIN, IUT Aix-Marseille, dept. Informatique - Arles
//TP simple, affichages de quelques points
//compilation par : g++ -Wall TP1points.cpp -lGL -lGLU -lglut -lGLEW -o TP1points

#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <random>
#include <memory>

using namespace std;

GLuint leVBO;
size_t nb_vertices;

static void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);


//Liaison avec le buffer de vertex
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, leVBO);
    glVertexPointer(3, GL_FLOAT, 0, 0); //description des données pointées

    glDrawArrays(GL_POINTS, 0, nb_vertices); //3 éléments à utiliser pour le dessin

	glDisableClientState(GL_VERTEX_ARRAY); //plus besoin de vertexArray

	glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderScene);
}

static void CreateVertexBufferStar()
{
    float vertices[27]; // 3 points à 3 coordonnées x,y,z par point
    nb_vertices = 9;

//1er sommet
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = 0.0f;

//2ème sommet
	vertices[3] = 0.25f;
	vertices[4] = 0.25f;
	vertices[5] = 0.0f;

//3ème sommet
	vertices[6] = 0.0f;
	vertices[7] = 0.9f;
	vertices[8] = 0.0f;

    vertices[9] = -0.25f; //pour tester
	vertices[10] = 0.25f;
	vertices[11] = 0.0f;

	vertices[12] = -.9f;
	vertices[13] = 0.0f;
	vertices[14] = 0.0f;

	vertices[15] = -0.25f;
	vertices[16] = -0.25f;
	vertices[17] = 0.0f;

	vertices[18] = 0.0f;
	vertices[19] = -0.9f;
	vertices[20] = 0.0f;

	vertices[21] = 0.25f;
	vertices[22] = -0.25f;
	vertices[23] = 0.0f;

	vertices[24] = 0.9f;
	vertices[25] = 0.0f;
    vertices[26] = 0.0f;

 	glGenBuffers(1, &leVBO); //génération d'une référence de buffer object
	glBindBuffer(GL_ARRAY_BUFFER, leVBO); //liaison du buffer avec un type tableau de données
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*9, vertices, GL_STATIC_DRAW); //création et initialisation du container de données (3 sommets -> 9 float)
}

void CreateVertexBufferRandom(const size_t nb_points)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.f, 1.f);

    nb_vertices = nb_points;

    auto vertices = std::make_unique<float[]>(nb_points * 3);

    for (size_t i = 0; i < nb_points * 3; ++i)
        vertices[i] = dis(gen);

    glGenBuffers(1, &leVBO);
    glBindBuffer(GL_ARRAY_BUFFER, leVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * nb_points, vertices.get(), GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TP1 : quelques points");

	InitializeGlutCallbacks();

	//toujours après l'initialisation de glut
	GLenum res = glewInit();

	if (res != GLEW_OK) {
		cerr << "Error: " << glewGetErrorString(res) << endl;
		return (EXIT_FAILURE);
	}

	//cout << "Using GLEW Version: " << glewGetString(GLEW_VERSION);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

//	CreateVertexBufferStar();
    CreateVertexBufferRandom(10);

	glutMainLoop();

	return (EXIT_SUCCESS);
}
