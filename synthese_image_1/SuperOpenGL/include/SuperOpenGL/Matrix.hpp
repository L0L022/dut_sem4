#ifndef SUPEROPENGL_MATRIX_HPP
#define SUPEROPENGL_MATRIX_HPP

#include <array>
#include <functional>
#include <iostream>

namespace SuperOpenGL {

template<typename T, size_t M, size_t N>
class Matrix {
    using Array = std::array<T, M * N>;
public:
    Matrix()
    {
        fill(0);
    }

    Matrix(std::initializer_list<T> l) { std::copy(l.begin(), l.end(), _elements.begin()); }

    Matrix(const Matrix &m) = default;

    Matrix(Matrix &&m)
        : _elements(std::move(m._elements))
    {
    }

    Matrix& operator=(const Matrix &m) = default;

    Matrix& operator=(std::initializer_list<T> l) {
        std::copy(l.begin(), l.end(), _elements.begin());
    }

    size_t rows() const { return M; }
    size_t columns() const { return N; }
    size_t size() const { return M * N; }

    const T &element(const size_t row, const size_t column) const {
        return _elements[this->indexOfElement(row, column)];
    }

    T &element(const size_t row, const size_t column) {
        return _elements[this->indexOfElement(row, column)];
    }

    const T &operator()(const size_t row, const size_t column) const {
        return element(row, column);
    }

    T &operator()(const size_t row, const size_t column) {
        return element(row, column);
    }

    T * elements() { return _elements.data(); }
    const T * elements() const { return _elements.data(); }


    bool operator==(const Matrix &m) const {
        return std::equal(begin(), end(), m.begin(), m.end());
    }

    bool operator !=(const Matrix &m) const {
        return !(*this == m);
    }

    Matrix operator+(const T &scalar) const {
        Matrix m(*this);
        this->addition(m, scalar);
        return m;
    }

    Matrix &operator+=(const T &scalar) {
        this->addition(*this, scalar);
        return *this;
    }

    Matrix operator+(const Matrix &matrix) const {
        Matrix m(*this);
        this->addition(m, matrix);
        return m;
    }

    Matrix &operator+=(const Matrix &matrix) {
        this->addition(*this, matrix);
        return *this;
    }

    Matrix operator-(const T &scalar) const {
        Matrix m(*this);
        this->substraction(m, scalar);
        return m;
    }

    Matrix &operator-=(const T &scalar) {
        this->substraction(*this, scalar);
        return *this;
    }

    Matrix operator-(const Matrix &matrix) const {
        Matrix m(*this);
        this->substraction(m, matrix);
        return m;
    }

    Matrix &operator-=(const Matrix &matrix) {
        this->substraction(*this, matrix);
        return *this;
    }

    Matrix operator*(const T &scalar) const {
        Matrix m(*this);
        this->multiplication(m, scalar);
        return m;
    }

    Matrix &operator*=(const T &scalar) {
        this->multiplication(*this, scalar);
        return *this;
    }

    template <size_t N2>
    Matrix<T, M, N2> operator*(const Matrix<T, N, N2> &matrix) const {
        return this->multiplication(*this, matrix);
    }

    Matrix operator/(const T &scalar) const {
        Matrix m(*this);
        this->division(m, scalar);
        return m;
    }

    Matrix &operator/=(const T &scalar) {
        this->division(*this, scalar);
        return *this;
    }

    typename Array::iterator begin() { return _elements.begin(); }
    typename Array::const_iterator begin() const { return _elements.begin(); }
    typename Array::const_iterator cbegin() const { return _elements.cbegin(); }

    typename Array::iterator end() { return _elements.end(); }
    typename Array::const_iterator end() const  { return _elements.end(); }
    typename Array::const_iterator cend() const  { return _elements.cend(); }

    void fill(const T &e) {
        std::fill(begin(), end(), e);
    }

protected:
    size_t indexOfElement(const size_t row, const size_t column) const {
        if (1 <= row and row <= rows() and 1 <= column and column <= columns())
            return (row - 1) * columns() + (column - 1);
        else
            throw std::runtime_error("Matrix : row or column out of bounds.");
    }

private:
    static void transform(Matrix &m, std::function<T(const T&, size_t, size_t)> op) {
        for (size_t i = 1; i <= m.rows(); ++i)
            for (size_t j = 1; j <= m.columns(); ++j)
                m.element(i, j) = op(m.element(i, j), i, j);
    }

    static void addition(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e + scalar;
        });
    }

    static void addition(Matrix &m1, const Matrix &m2) {
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
        transform(m1, [&m2](const T& e, size_t i, size_t j) {
            return e - m2.element(i, j);
        });
    }

    static void multiplication(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e * scalar;
        });
    }

    template <size_t M1, size_t N2, size_t O>
    static Matrix<T, M1, N2> multiplication(Matrix<T, M1, O> &m1, const Matrix<T, O, N2> &m2) {
        Matrix<T, M1, N2> m;
        transform(m, [&m1, &m2](const T&, size_t i, size_t j) {
            T r = m1.element(i, 1) * m2.element(1, j);
            for (size_t k = 2; k < m1.columns(); ++k)
                r += m1.element(i, k) * m2.element(k, j);
            return r;

        });
        return m;
    }

    static void division(Matrix &m, const T &scalar) {
        transform(m, [&scalar](const T &e, size_t, size_t) {
            return e / scalar;
        });
    }

private:
    Array _elements;
};

template<typename T, size_t M, size_t N>
std::ostream &operator<<(std::ostream &os, const Matrix<T, M, N> &m) {
    for (size_t i = 1; i <= m.rows(); ++i) {
        for (size_t j = 1; j <= m.columns(); ++j) {
            os << m.element(i, j);
            if (j != m.columns())
                os << ' ';
        }
        if (i != m.rows())
            os << '\n';
    }
    return os;
}

}

#endif
