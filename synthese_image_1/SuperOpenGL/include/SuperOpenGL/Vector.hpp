#ifndef SUPEROPENGL_VECTOR_HPP
#define SUPEROPENGL_VECTOR_HPP

#include <iostream>

namespace SuperOpenGL {

class Vertex;

class Vector
{
public:
  long double x;
  long double y;
  long double z;

  Vector(); // (0,0,0) par defaut
  Vector(const long double&, const long double&, const long double&);
  Vector(const Vertex&);
  Vector(const Vertex&, const Vertex&);
  Vector(const Vector&);
  ~Vector();

  bool operator==(const Vector& op) const;
  Vector& operator=(const Vector& op);
  Vector& operator=(const Vertex& op);
  Vector operator+() const;
  Vector operator+(const Vector&) const;
  Vector operator+(const Vertex&) const;

  Vector& operator+=(const Vector& op);
  Vector operator-() const;
  Vector operator-(const Vector& op) const;
  Vector& operator-=(const Vector& op);
  Vector& operator*=(const long double op);      // produit par un scalaire
  Vector operator*(const long double op) const;  // idem
  Vector& operator/=(const long double op);      // division par un scalaire
  Vector operator/(const long double op) const;  // idem
  long double operator*(const Vector& op) const; // produit scalaire
  Vector& operator^=(const Vector& op);          // produit vectoriel
  Vector operator^(const Vector& op) const;

  Vector normalize(
    const long double lg);    // normalise un vecteur a la longueur lg
  long double length() const; // longueur du vecteur

  friend std::ostream& operator<<(std::ostream&, const Vector&);
  friend std::istream& operator>>(std::istream&, Vector&);
};

} // namespace lib

#endif
