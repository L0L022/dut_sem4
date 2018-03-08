#ifndef SUPEROPENGL_REGULARTETRAHEDRONGEOMETRY_HPP
#define SUPEROPENGL_REGULARTETRAHEDRONGEOMETRY_HPP

#include <SuperOpenGL/Geometry.hpp>

namespace SuperOpenGL {

class RegularTetrahedronGeometry : public Geometry {
public:
    RegularTetrahedronGeometry(const Vector3f &center, const float edgeLength);

private:
    void make(const Vector3f &center, const float edgeLength);
};

}

#endif
