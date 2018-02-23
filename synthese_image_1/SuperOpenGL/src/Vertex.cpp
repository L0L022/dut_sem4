#include <iostream>
#include <SuperOpenGL/Vector.hpp>
#include <SuperOpenGL/Vertex.hpp>

using namespace SuperOpenGL;
using namespace std;

/*!
 *   \brief constructeur sans paramètres
 *   \todo none
 *   \note RR: modif 19/12/2006 : =0 -> =0.0 pour les long doubles
 */
Vertex::Vertex()
  : x(0.0)
  , y(0.0)
  , z(0.0)
{
}

/*!
 *   \brief constructeur à partir de 3 réels (long double)
 *   \todo none
 */
Vertex::Vertex(const long double x, const long double y, const long double z)
  : x(x)
  , y(y)
  , z(z)
{
}

/*!
 *   \brief constructeur de copie
 *   \todo none
 */
Vertex::Vertex(const Vertex& p)
  : x(p.x)
  , y(p.y)
  , z(p.z)
{
}

/*!
 *   \brief constructeur de copie de vecteur3
 *   \todo none
 */
Vertex::Vertex(const Vector& v)
  : x(v.x)
  , y(v.y)
  , z(v.z)
{
}

/*!
 *   \brief destructeur
 *   \todo none
 */
Vertex::~Vertex() {}

/*!
 *   \brief conditionnelle ==
 *   \todo none
 */
bool
Vertex::operator==(const Vertex& p) const
{
  return x == p.x && y == p.y && z == p.z;
}

/*!
 *   \brief affectation
 *   \todo none
 */
Vertex&
Vertex::operator=(const Vertex& p)
{
  x = p.x;
  y = p.y;
  z = p.z;

  return *this;
}

/*!
 *   \brief affectation
 *   \todo none
 */
Vertex&
Vertex::operator=(const Vector& v)
{
  x = v.x;
  y = v.y;
  z = v.z;

  return *this;
}

/*!
 *   \brief addition Vertex+vecteur
 *   \todo none
 */
Vertex
Vertex::operator+(const Vector& v) const
{
  return Vertex(x + v.x, y + v.y, z + v.z);
}

/*!
 *   \brief addition Vertex+Vertex
 *   \todo none
 */
Vertex
Vertex::operator+(const Vertex& p) const
{
  return Vertex(x + p.x, y + p.y, z + p.z);
}

/*!
 *   \brief combinaison affectation/addition avec un vecteur (translation)
 *   \todo none
 */
Vertex&
Vertex::operator+=(const Vector& v)
{
  x += v.x;
  y += v.y;
  z += v.z;

  return *this;
}

/*!
 *   \brief soustraction de Vertexs
 *   \todo none
 */
Vertex
Vertex::operator-(const Vertex& p) const
{
  return Vertex(x - p.x, y - p.y, z - p.z);
}

/*!
 *   \brief combinaison affectation/multiplication par un scalaire
 *   \todo none
 */
Vertex&
Vertex::operator*=(const long double n)
{
  x *= n;
  y *= n;
  z *= n;

  return *this;
}

/*!
 *   \brief multiplication par un scalaire (mise à l'échelle)
 *   \todo none
 */
Vertex Vertex::operator*(const long double n) const
{
  return Vertex(x * n, y * n, z * n);
}

/*!
 *   \brief combinaison affectation/division par un scalaire
 *   \todo none
 *   \note RR: modif 19/12/2006 : test sur op!=0
 */
Vertex&
Vertex::operator/=(const long double n)
{
  if (n == 0.0) throw runtime_error("Division by zero");

  x /= n;
  y /= n;
  z /= n;

  return *this;
}

/*!
 *   \brief division par un scalaire
 *   \todo none
 */
Vertex
Vertex::operator/(const long double n) const
{
  if (n == 0.0) throw runtime_error("Division by zero");

  return Vertex(x / n, y / n, z / n);
}

/*!
 *   \brief multiplication par un Vertex (produit scalaire)
 *   \todo none
 */
Vertex Vertex::operator*(const Vertex& p) const
{
  return Vertex(x * p.x, y * p.y, z * p.z);
}

/*!
 *   \brief combinaison affectation/multiplication par un Vertex
 *   \todo none
 */
Vertex&
Vertex::operator*=(const Vertex& p)
{
  x *= p.x;
  y *= p.y;
  z *= p.z;

  return *this;
}

/*!
 *   \brief division par un Vertex
 *   \todo none
 */
Vertex
Vertex::operator/(const Vertex& p) const
{
  if (p.x == 0.0 || p.y == 0.0 || p.z == 0.0)
    throw runtime_error("Division by zero");

  return Vertex(x / p.x, y / p.y, z / p.z);
}

/*!
 *   \brief combinaison affectation/division par un Vertex
 *   \todo none
 */
Vertex&
Vertex::operator/=(const Vertex& p)
{
  if (p.x == 0.0 || p.y == 0.0 || p.z == 0.0)
    throw runtime_error("Division by zero");

  x /= p.x;
  y /= p.y;
  z /= p.z;

  return *this;
}

/*!
 *   \brief ecriture dans un flux ostream "(x, y, z)"
 *   \todo none
 */
namespace lib {

ostream&
operator<<(ostream& _os, const Vertex& _p)
{
  return _os << _p.x << ' ' << _p.y << ' ' << _p.z;
}

/*!
 *   \brief lecture dans un flux istream "Entrez x:\n ...y:\n ...z:"
 *   \todo none
 */
istream&
operator>>(istream& p, Vertex& op)
{
  return p >> op.x >> op.y >> op.z;
}

} // namespace lib
