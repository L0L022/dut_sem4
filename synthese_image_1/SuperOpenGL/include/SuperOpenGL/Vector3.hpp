#ifndef SUPEROPENGL_VECTOR3_HPP
#define SUPEROPENGL_VECTOR3_HPP

#include <SuperOpenGL/Matrix.hpp>

#include <cmath>

namespace SuperOpenGL {
/*
template<typename T>
class Vector3 : public Vector<T, 3>
{
public:
    Vector3() = default;
    Vector3(const T x, const T y, const T z) {
        this->x() = x;
        this->y() = y;
        this->z() = z;
    }
    Vector3(std::initializer_list<T> l) : Vector<T, 3>(l) {}
    Vector3(const Vector3 &v) = default;
    Vector3(Vector3 &&v) = default;

    Vector3& operator=(const Vector3 &v) = default;
    Vector3& operator=(std::initializer_list<T> l) {
        Vector<T, 3>::operator=(l);
        return *this;
    }

    T &x() { return this->element(1, 1); }
    const T &x() const { return this->element(1, 1); }

    T &y() { return this->element(2, 1); }
    const T &y() const { return this->element(2, 1); }

    T &z() { return this->element(3, 1); }
    const T &z() const { return this->element(3, 1); }

    Vector3& operator^=(const Vector3& v) {
        crossProduct(*this, v);
        return *this;
    }

    Vector3 operator^(const Vector3& v) const {
        Vector3 r(*this);
        crossProduct(r, v);
        return r;
    }

private:
    static void crossProduct(Vector3 &v1, const Vector3 &v2) {
        v1 = {v1.y() * v2.z() - v1.z() * v2.y(), v1.z() * v2.x() - v1.x() * v2.z(), v1.x() * v2.y() - v1.y() * v2.x()};
    }
};
*/

template <typename T>
class Vector3
{
public:
  T x;
  T y;
  T z;

  Vector3() : Vector3(0, 0, 0) {}
  Vector3(const T &x, const T& y, const T &z) : x(x), y(y), z(z) {}
  Vector3(const Vector3&) = default;
  Vector3(std::initializer_list<T> l) {
      //check size
      x = l.begin()[0];
      y = l.begin()[1];
      z = l.begin()[2];
  }
  Vector3(Matrix<T, 3, 1>& m) : Vector3(m(1, 1), m(2, 1), m(3, 1)) {}
  Vector3(Matrix<T, 1, 3>& m) : Vector3(m(1, 1), m(1, 2), m(1, 3)) {}

  Vector3& operator=(const Vector3& v) = default;
  Vector3& operator=(std::initializer_list<T> l) {
      //check size
      x = l.begin()[0];
      y = l.begin()[1];
      z = l.begin()[2];
      return *this;
  }
  Vector3& operator=(Matrix<T, 3, 1>& m) {
      x = m(1, 1);
      y = m(2, 1);
      z = m(3, 1);
      return *this;
  }
  Vector3& operator=(Matrix<T, 1, 3>& m) {
      x = m(1, 1);
      y = m(1, 2);
      z = m(1, 3);
      return *this;
  }

  bool operator==(const Vector3& v) const {
      return x == v.x && y == v.y && z == v.z;
  }

  Vector3 operator+(const Vector3& v) const {
      return {x + v.x, y + v.y, z + v.z};
  }

  Vector3& operator+=(const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  Vector3 operator-() const {
      return {-x, -y, -z};
  }

  Vector3 operator-(const Vector3& v) const {
      return {x - v.x, y - v.y, z - v.z};
  }

  Vector3& operator-=(const Vector3& v) {
      x -= v.x;
      y -= v.y;
      z -= v.z;
    return *this;
  }

  Vector3& operator*=(const T s) {
      x *= s;
      y *= s;
      z *= s;
    return *this;
  }

  Vector3 operator*(const T s) const {
      return {x * s, y * s, z * s};
  }

  Vector3& operator/=(const T s) {
      x /= s;
      y /= s;
      z /= s;
    return *this;
  }

  Vector3 operator/(const T s) const {
      return {x / s, y / s, z / s};
  }

  T operator*(const Vector3& v) const {
      return x * v.x + y * v.y + z * v.z;
  }

  Vector3& operator^=(const Vector3& v) {
    x = y * v.z - z * v.y;
    y = z * v.x - x * v.z;
    z = x * v.y - y * v.x;
    return *this;
  }

  Vector3 operator^(const Vector3& v) const {
      return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
  }

  Vector3 normalize(const T lg = 1) {
      T lgtemp;
        if (x != 0 || y != 0 || z != 0) {
          lgtemp = std::sqrt(x * x + y * y + z * z);
          lgtemp = lg / lgtemp;
          x *= lgtemp;
          y *= lgtemp;
          z *= lgtemp;
        }
      return *this;
  }

  T length() const {
      return std::sqrt(x * x + y * y + z * z);
  }

  Matrix<T, 3, 1> rowMatrix() const {
      return {x, y, z};
  }

  Matrix<T, 1, 3> columnMatrix() const {
      return {x, y, z};
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
    return os << v.x << ' ' << v.y << ' ' << v.z;
}

template <typename T>
std::istream& operator>>(std::istream& is, Vector3<T> & v) {
     return is >> v.x >> v.y >> v.z;
}

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

} // namespace lib

#endif

