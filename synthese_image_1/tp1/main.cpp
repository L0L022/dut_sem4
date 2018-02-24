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
#include <algorithm>
#include <functional>

#include <SuperOpenGL/SuperOpenGL.hpp>
#include <SuperOpenGL/Window.hpp>

using namespace std;

GLuint leVBO;
size_t nb_vertices;

template<typename T>
using Vector3 = T[3];

template<typename T>
using ArrayVector3 = std::vector<Vector3<T>>;

template<typename T>
class Matrix {
    template<typename U>
    using Array = std::vector<U>;

public:
    using iterator = typename Array<T>::iterator;
    using const_iterator = typename Array<T>::const_iterator;

    Matrix()
        : Matrix(1, 1)
    {}

    Matrix(size_t rows, size_t columns)
        : _fixed(false), _rows(0), _columns(0)
    {
        setSize(rows, columns);
    }

    Matrix(const Matrix &m) = default;

    Matrix(Matrix &&m)
        : _rows(0), _columns(0)
    {
        std::swap(_rows, m._rows);
        std::swap(_columns, m._columns);
        std::swap(_elements, m._elements);
    }

    Matrix& operator=(const Matrix &m) = default;

    inline size_t rows() const { return _rows; }
    inline size_t columns() const { return _columns; }
    inline size_t size() const { return _rows * _columns; }

    inline const T &element(const size_t row, const size_t column) const {
        return _elements[indexOfElement(row, column)];
    }
    inline T &element(const size_t row, const size_t column) {
        return _elements[indexOfElement(row, column)];
    }

    inline T * elements() { return _elements.data(); }
    inline const T * elements() const { return _elements.data(); }

    bool operator==(const Matrix &m) const {
        return _rows == m._rows and _columns == m._columns and _elements == m._elements;
    }

    bool operator !=(const Matrix &m) const {
        return !(*this == m);
    }

    iterator begin() { return _elements.begin(); }
    const_iterator begin() const { return _elements.begin(); }
    const_iterator cbegin() const { return _elements.cbegin(); }

    iterator end() { return _elements.end(); }
    const_iterator end() const { return _elements.end(); }
    const_iterator cend() const { return _elements.cend(); }

    Matrix operator+(const T &scalar) const {
        Matrix m(*this);
        addition(m, scalar);
        return m;
    }

    Matrix &operator+=(const T &scalar) {
        addition(*this, scalar);
        return *this;
    }

    Matrix operator+(const Matrix &matrix) const {
        Matrix m(*this);
        addition(m, matrix);
        return m;
    }

    Matrix &operator+=(const Matrix &matrix) {
        addition(*this, matrix);
        return *this;
    }

    Matrix operator-(const T &scalar) const {
        Matrix m(*this);
        substraction(m, scalar);
        return m;
    }

    Matrix &operator-=(const T &scalar) {
        substraction(*this, scalar);
        return *this;
    }

    Matrix operator-(const Matrix &matrix) const {
        Matrix m(*this);
        substraction(m, matrix);
        return m;
    }

    Matrix &operator-=(const Matrix &matrix) {
        substraction(*this, matrix);
        return *this;
    }

    Matrix operator*(const T &scalar) const {
        Matrix m(*this);
        multiplication(m, scalar);
        return m;
    }

    Matrix &operator*=(const T &scalar) {
        multiplication(*this, scalar);
        return *this;
    }

    Matrix operator*(const Matrix &matrix) const {
        Matrix m(*this);
        multiplication(m, matrix);
        return m;
    }

    Matrix &operator*=(const Matrix &matrix) {
        multiplication(*this, matrix);
        return *this;
    }

    Matrix operator/(const T &scalar) const {
        Matrix m(*this);
        division(m, scalar);
        return m;
    }

    Matrix &operator/=(const T &scalar) {
        division(*this, scalar);
        return *this;
    }

private:
    inline size_t indexOfElement(const size_t row, const size_t column) const {
        if (1 <= row and row <= _rows and 1 <= column and column <= _columns)
            return (row - 1) * _rows + (column - 1);
        else
            throw std::runtime_error("Matrix : row or column out of bounds.");
    }

    void setSize(const size_t rows, const size_t columns) {
        if (_fixed)
            throw std::runtime_error("Matrix : the change of dimension is forbidden.");

        if (rows < 1 or columns < 1)
            throw std::runtime_error("Matrix : rows or columns are inferior to zero.");

        _rows = rows;
        _columns = columns;
        _elements.resize(size()); // ne coupe pas la matrice comme on pourrait le penser
    }

    static void transform(Matrix &m, std::function<T(const T&, size_t, size_t)> op) {
        for (size_t i = 1; i <= m.rows(); ++i)
            for (size_t j = 1; j <= m.columns(); ++j)
                m.element(i, j) = op(m.element(i, j), i, j);
    }

    static void checkSameDimension(const Matrix &m1, const Matrix &m2) {
        if (m1.rows() != m2.rows() or m1.columns() != m2.columns())
            throw std::runtime_error("Matrix : the two matrix must have the same dimension.");
    }

    static void addition(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e + scalar;
        });
    }

    static void addition(Matrix &m1, const Matrix &m2) {
        checkSameDimension(m1, m2);
        transform(m1, [&m2](const T& e, size_t i, size_t j) {
            return e + m2.element(i, j);
        });
    }

    static void substraction(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e - scalar;
        });
    }

    static void substraction(Matrix &m1, const Matrix &m2) {
        checkSameDimension(m1, m2);
        transform(m1, [&m2](const T& e, size_t i, size_t j) {
            return e - m2.element(i, j);
        });
    }

    static void multiplication(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e * scalar;
        });
    }

    static void multiplication(Matrix &m1, const Matrix &m2) {
        if (m1.columns() != m2.rows())
            throw std::runtime_error("Matrix : error with dimension");
        Matrix m(m1.rows(), m2.columns());
        transform(m, [&m1, &m2](const T& e, size_t i, size_t j) {
            T r = m1.element(i, 1) * m2.element(1, j);
            for (size_t k = 2; k < m1.columns(); ++k)
                r += m1.element(i, k) * m2.element(k, j);
            return r;

        });
        m1 = m;
    }

    static void division(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e / scalar;
        });
    }

protected:
    bool _fixed;

private:
    size_t _rows;
    size_t _columns;
    std::vector<T> _elements;
};

template<typename T, size_t dimension>
class Vector : Matrix<T> {
public:
    Vector() : Matrix<T>(), _fixed(true) {}
};

class Geometry {
public:

};

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

class MainWindow : public SuperOpenGL::Window {
public:
    MainWindow() : SuperOpenGL::Window("TP1 : quelques points")
    {}

protected:
    void display() {
        RenderScene();
    }
};

int main(int argc, char** argv)
{
    SuperOpenGL::init(argc, argv);
    //toujours après l'initialisation de glut
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        cerr << "Error: " << glewGetErrorString(res) << endl;
        return (EXIT_FAILURE);
    }

    MainWindow window;
    window.setPosition(200, 200);
    window.resize(500, 500);
    CreateVertexBufferRandom(100);

    MainWindow w2;
    MainWindow w3;


	//cout << "Using GLEW Version: " << glewGetString(GLEW_VERSION);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

//	CreateVertexBufferStar();

    Matrix<float> m;

    SuperOpenGL::mainLoop();

	return (EXIT_SUCCESS);
}
