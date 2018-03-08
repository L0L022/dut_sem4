#ifndef SUPEROPENGL_GEOMETRY_HPP
#define SUPEROPENGL_GEOMETRY_HPP

#include <SuperOpenGL/Vector3.hpp>

#include <vector>

namespace SuperOpenGL {

class Geometry {
public:
    size_t verticesCount() const;
    const Vector3f * vertices() const;

protected:
    std::vector<Vector3f> _vertices;
};

}

#endif
