#pragma once
#include <string>
#include <GL/glew.h>
#include <SOIL.h>

class Texture
{
public:
	Texture(const std::string& filePath);
	~Texture();

	void Bind(GLenum textureUnit = GL_TEXTURE0) const;
	GLuint GetID() const { return id; }
private:
	GLuint id = 0;
};

