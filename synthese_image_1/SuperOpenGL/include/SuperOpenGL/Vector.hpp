#ifndef SUPEROPENGL_VECTOR_HPP
#define SUPEROPENGL_VECTOR_HPP

#include <SuperOpenGL/Matrix.hpp>
#include <cmath>

namespace SuperOpenGL {

template<typename T, size_t dimension>
class Vector : public Matrix<T, dimension, 1>
{
public:
    Vector() = default;
    Vector(std::initializer_list<T> l) : Matrix<T, dimension, 1>(l) {}

    Vector(const Vector &v) = default;
    Vector(Vector &&v) = default;

    Vector& operator=(const Vector &v) = default;

    Vector& operator=(std::initializer_list<T> l) {
        Matrix<T, dimension, 1>::operator=(l);
        return *this;
    }

    const T &operator[](const size_t i) const { return this->element(i, 1); }
    T &operator[](const size_t i) { return this->element(i, 1); }

    Vector normalize(const T lg = 1) {
        T lgtemp;
//        if (x != 0 || y != 0 || z != 0) {
          lgtemp = length();
          lgtemp = lg / lgtemp;
          for (size_t i = 1; i <= dimension; ++i)
              this->element(i, 1) *= lgtemp;
//        }
        return (*this);
    }

    T length() const {
        T r = 0;
        for (size_t i = 1; i <= dimension; ++i)
            r += this->element(i, 1) ^ 2;
        return std::sqrt(r);
    }

};

}


#endif
