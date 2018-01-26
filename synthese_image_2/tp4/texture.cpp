#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : id(0), width(0), height(0) {

}

bool Texture::charger(const std::string &fileName)
{
    glGenTextures(1, &id);
    definir_filtrage(GL_NEAREST, GL_NEAREST);
    definir_bouclage(GL_REPEAT, GL_REPEAT);
    definir_melange(GL_MODULATE);

    int comp;
    img.reset(stbi_load(fileName.c_str(), &width, &height, &comp, 0));
    if (img == nullptr)
        return false;

    GLenum format;
    switch (comp) {
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        return false;
    }
    utiliser();
    glTexImage2D(GL_TEXTURE_2D, 0, comp, width, height, 0, format, GL_UNSIGNED_BYTE, img.get());
    return true;
}

void Texture::utiliser() {
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::definir_filtrage(GLint mode_min, GLint mode_mag) {
    utiliser();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode_mag);
}

void Texture::definir_bouclage(GLint mode_axe_s, GLint mode_axe_t) {
    utiliser();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_axe_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_axe_t);
}

void Texture::definir_melange(GLint mode) {
    utiliser();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
}
