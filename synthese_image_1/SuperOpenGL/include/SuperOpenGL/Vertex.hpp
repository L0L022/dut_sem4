#ifndef SUPEROPENGL_VERTEX_HPP
#define SUPEROPENGL_VERTEX_HPP

#include <iostream>

namespace SuperOpenGL {

class Vector;

class Vertex
{
public:
  long double x;
  long double y;
  long double z;

  Vertex();
  Vertex(const long double x, const long double y, const long double z);
  Vertex(const Vertex&);
  Vertex(const Vector&);
  ~Vertex();

  bool operator==(const Vertex&) const;

  Vertex& operator=(const Vertex&);
  Vertex& operator=(const Vector&);

  Vertex operator+(const Vertex&) const;
  Vertex operator+(const Vector&) const;
  Vertex& operator+=(const Vector&);

  Vertex operator-(const Vertex&) const;

  Vertex operator*(const Vertex&)const;
  Vertex& operator*=(const Vertex&);

  Vertex operator*(const long double)const;
  Vertex& operator*=(const long double);

  Vertex operator/(const long double) const;
  Vertex& operator/=(const long double);

  Vertex operator/(const Vertex&) const;
  Vertex& operator/=(const Vertex&);

  friend std::ostream& operator<<(std::ostream&, const Vertex&);
  friend std::istream& operator>>(std::istream&, Vertex&);
};

} // namespace lib

#endif
