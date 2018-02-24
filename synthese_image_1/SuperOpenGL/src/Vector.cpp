#include <cmath>
#include <SuperOpenGL/Vector.hpp>
#include <SuperOpenGL/Vertex.hpp>

using namespace SuperOpenGL;

Vector::Vector()
{
  // modif RR, 05/12/2006, passage 0.0 -> 0.0f
  x = 0.0;
  y = 0.0;
  z = 0.0;
}

// modif RR 02/12/2006, parametre const long double -> const long double&
Vector::Vector(const long double& X, const long double& Y, const long double& Z)
{
  x = X;
  y = Y;
  z = Z;
}

/*!
 * \author RR
 * \date 05/12/2006
 */
Vector::Vector(const Vector& _v)
{
  x = _v.x;
  y = _v.y;
  z = _v.z;
}

/*!
 * \author RR
 * \date 05/12/2006
 */
Vector::~Vector()
{
  // rien
}

/*!
 * \author RR
 * \date 05/12/2006
 */
Vector::Vector(const Vertex& _P)
{
  x = _P.x;
  y = _P.y;
  z = _P.z;
}

/*!
 * \author RR
 * \date 05/12/2006
 */
Vector::Vector(const Vertex& _A, const Vertex& _B)
{
  x = _B.x - _A.x;
  y = _B.y - _A.y;
  z = _B.z - _A.z;
}

bool
Vector::operator==(const Vector& op) const
{
  return (x == op.x && y == op.y && z == op.z);
}

Vector&
Vector::operator=(const Vector& op)
{
  x = op.x;
  y = op.y;
  z = op.z;
  return *this;
}

Vector&
Vector::operator=(const Vertex& op)
{
  x = op.x;
  y = op.y;
  z = op.z;

  return *this;
}

Vector
Vector::operator+() const
{
  return (*this);
}

Vector
Vector::operator+(const Vector& op) const
{
  return (Vector(x + op.x, y + op.y, z + op.z));
}

/*! RR 02Juillet2008
 */
Vector
Vector::operator+(const Vertex& _p) const
{
  return (Vector(x + _p.x, y + _p.y, z + _p.z));
}

Vector&
Vector::operator+=(const Vector& op)
{
  x += op.x;
  y += op.y;
  z += op.z;
  return *this;
}

Vector
Vector::operator-() const
{
  return (Vector(-x, -y, -z));
}

Vector
Vector::operator-(const Vector& op) const
{
  return (Vector(x - op.x, y - op.y, z - op.z));
}

Vector&
Vector::operator-=(const Vector& op)
{
  x -= op.x;
  y -= op.y;
  z -= op.z;
  return *this;
}

Vector&
Vector::operator*=(const long double op)
{
  x *= op;
  y *= op;
  z *= op;
  return *this;
}

Vector Vector::operator*(const long double op) const
{
  return (Vector(x * op, y * op, z * op));
}

Vector&
Vector::operator/=(const long double op)
{
  x /= op;
  y /= op;
  z /= op;
  return *this;
}

Vector
Vector::operator/(const long double op) const
{
  return (Vector(x / op, y / op, z / op));
}

long double Vector::operator*(const Vector& op) const
{
  return (x * op.x + y * op.y + z * op.z);
}

Vector&
Vector::operator^=(const Vector& op)
{
  Vector temp(y * op.z - z * op.y, z * op.x - x * op.z, x * op.y - y * op.x);
  x = temp.x;
  y = temp.y;
  z = temp.z;
  return *this;
}

Vector
Vector::operator^(const Vector& op) const
{
  return (
    Vector(y * op.z - z * op.y, z * op.x - x * op.z, x * op.y - y * op.x));
}

Vector
Vector::normalize(const long double lg)
{
  long double lgtemp;
  if (x != 0 || y != 0 || z != 0) {
    lgtemp = sqrt(x * x + y * y + z * z);
    lgtemp = lg / lgtemp;
    x *= lgtemp;
    y *= lgtemp;
    z *= lgtemp;
  }
  return (*this);
}

long double
Vector::length() const
{
  return (sqrt(x * x + y * y + z * z));
}

namespace lib {

std::ostream&
operator<<(std::ostream& os, const Vector& v)
{
  return os << v.x << ' ' << v.y << ' ' << v.z;
}

std::istream&
operator>>(std::istream& is, Vector& v)
{
  return is >> v.x >> v.y >> v.z;
}

} // namespace lib
