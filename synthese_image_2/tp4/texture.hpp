#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <memory>
#include <GL/glu.h>

class Texture {
private:
    GLuint id;
    int width, height;
    std::unique_ptr<unsigned char[]> img;

public:
    Texture();

    bool charger(const std::string &fileName);
    void utiliser();
    void definir_filtrage(GLint mode_min, GLint mode_mag);
    void definir_bouclage(GLint mode_axe_s, GLint mode_axe_t);
    void definir_melange(GLint mode);
};

#endif
