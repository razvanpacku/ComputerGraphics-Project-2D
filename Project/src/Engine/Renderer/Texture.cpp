#include "./Texture.h"

#include <iostream>

Texture::Texture(const std::string& filePath)
{
    id = SOIL_load_OGL_texture(
        filePath.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
    );

    if (!id)
    {
        std::cerr << "SOIL failed to load texture: " << filePath << "\n";
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::cout << "Loaded texture: " << filePath << " (ID " << id << ")\n";
    }
}

Texture::~Texture()
{
    if (id)
        glDeleteTextures(1, &id);
}

void Texture::Bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}
