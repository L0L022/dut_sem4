#include <SuperOpenGL/CircleGeometry.hpp>

namespace SuperOpenGL {

CircleGeometry::CircleGeometry(const Vector3f &center, const float radius, const size_t nbVertices) {
    make(center, radius, nbVertices);
}

void CircleGeometry::make(const Vector3f &center, const float radius, const size_t nbVertices) {
    _vertices.resize(nbVertices);
    float t = 0;
    for (size_t i = 0; i < nbVertices; ++i, t += M_PI * 2.f / nbVertices)
        _vertices[i] = {center.x + std::cos(t) * radius, center.y + std::sin(t) * radius, center.z};
}

}
