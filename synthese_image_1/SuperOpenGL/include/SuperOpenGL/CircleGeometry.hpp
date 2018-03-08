#ifndef SUPEROPENGL_CIRCLEGEOMETRY_HPP
#define SUPEROPENGL_CIRCLEGEOMETRY_HPP

#include <SuperOpenGL/Geometry.hpp>

namespace SuperOpenGL {

class CircleGeometry : public Geometry {
public:
    CircleGeometry(const Vector3f &center, const float radius, const size_t nbVertices);

private:
    void make(const Vector3f &center, const float radius, const size_t nbVertices);
};

}

#endif
