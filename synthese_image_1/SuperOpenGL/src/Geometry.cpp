#include <SuperOpenGL/Geometry.hpp>

namespace SuperOpenGL {

size_t Geometry::verticesCount() const {
    return _vertices.size();
}

const Vector3f *Geometry::vertices() const {
    return _vertices.data();
}

}
