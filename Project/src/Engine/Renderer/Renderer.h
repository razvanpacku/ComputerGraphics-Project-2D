#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shader.h"

#define DEFAULT_CLEAR_COLOR_R 0.75f
#define DEFAULT_CLEAR_COLOR_G 1.0f
#define DEFAULT_CLEAR_COLOR_B 1.0f

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Init();

	void Clear() const;
	
	void Render() const;

	void Cleanup() const;

private:
	Shader* shader;

	//temporary static variables, move to another file later
	GLuint
		VaoId=0,
		VboId=0,
		ColorBufferId=0,
		ProgramId=0;

	void CreateVBO(void);
	void DestroyVBO(void) const;
	void CreateShaders(void);
	void DestroyShaders(void) const;
};

