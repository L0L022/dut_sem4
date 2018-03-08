#include <SuperOpenGL/RegularTetrahedronGeometry.hpp>
#include <cmath>

namespace SuperOpenGL {

RegularTetrahedronGeometry::RegularTetrahedronGeometry(const Vector3f &center, const float edgeLength)
{
    make(center, edgeLength);
}

void RegularTetrahedronGeometry::make(const Vector3f &center, const float edgeLength)
{
    float l = std::sqrt(edgeLength * edgeLength / 2.f);
    Vector3f A{-l, l, -l}, B{l, l, l}, C{l, -l, -l}, D{-l, -l, l};

    A += center;
    B += center;
    C += center;
    D += center;

    _vertices.resize(12);

    _vertices[0] = A;
    _vertices[1] = D;
    _vertices[2] = C;

    _vertices[3] = A;
    _vertices[4] = C;
    _vertices[5] = B;

    _vertices[6] = A;
    _vertices[7] = B;
    _vertices[8] = D;

    _vertices[9] = B;
    _vertices[10] = C;
    _vertices[11] = D;
}

}
